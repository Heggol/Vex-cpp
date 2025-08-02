#include "add.h"
#include "../lockfile.h"
#include "../package.h"
#include "../utils/utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;
using json = nlohmann::json;

struct PackageInstallStats {
    std::set<std::string> installedPackages; 
    long long totalUnpackedSize = 0;
    int totalPackagesCount = 0;
};

std::optional<json> fetchNpmMetadata(
    const std::string& packageName,
    const std::string& version
) {
    httplib::Client cli("https://registry.npmjs.org");
    cli.set_connection_timeout(30);
    cli.set_read_timeout(30);
    cli.set_follow_location(true);

    httplib::Result res = cli.Get(("/" + packageName).c_str());

    if (!res || res->status != 200) {
        if (res) {
            std::cerr << "ERROR: Failed to fetch metadata for " << packageName
                      << " - HTTP status: " << res->status << std::endl;
        } else {
            std::cerr << "ERROR: Failed to make request for " << packageName
                      << " - Error: " << httplib::to_string(res.error()) << std::endl;
        }
        return std::nullopt;
    }

    try {
        json metadata = json::parse(res->body);
        if (!metadata.is_object() || !metadata.contains("name") ||
            !metadata.contains("versions")) {
            std::cerr << "ERROR: Invalid metadata structure for " << packageName
                      << std::endl;
            return std::nullopt;
        }
        return metadata;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to parse JSON for " << packageName << ": "
                  << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<std::string> downloadTarball(const std::string& tarballUrl) {
    std::string fullUrl = tarballUrl;
    if (fullUrl.substr(0, 5) != "https") {
        fullUrl = "https" + fullUrl.substr(fullUrl.find("://"));
    }

    size_t hostStart = fullUrl.find("://") + 3;
    size_t pathStart = fullUrl.find('/', hostStart);

    std::string host = fullUrl.substr(hostStart, pathStart - hostStart);
    std::string path = fullUrl.substr(pathStart);

    httplib::Client cli("https://" + host);
    cli.set_connection_timeout(30);
    cli.set_read_timeout(30);
    cli.set_follow_location(true);

    httplib::Result res = cli.Get(path.c_str());

    if (!res || res->status != 200) {
        if (res) {
            std::cerr << "ERROR: Failed to download tarball from " << tarballUrl
                      << " - HTTP status: " << res->status << std::endl;
        } else {
            std::cerr << "ERROR: Failed to make request for tarball from "
                      << tarballUrl << " - Error: " << httplib::to_string(res.error())
                      << std::endl;
        }
        return std::nullopt;
    }
    return res->body;
}

void addPackages(
    const DepMap& packages,
    bool isRoot,
    int layer,
    const std::vector<bool>& prefixLines,
    bool firstDepType,
    PackageInstallStats& stats,
    const fs::path& projectRootPath
) {
    std::vector<std::pair<DepType, std::vector<std::string>>> depTypes = {
        {DepType::Dependencies, packages.dependencies},
        {DepType::DevDependencies, packages.devDependencies},
        {DepType::PeerDependencies, packages.peerDependencies}};

    for (const auto& depPair : depTypes) {
        DepType depType = depPair.first;
        const auto& pkgs = depPair.second;

        if (pkgs.empty()) {
            continue;
        }

        if (isRoot && firstDepType) {
            std::cout << (depType == DepType::DevDependencies ? "devDependencies"
                                                             : "dependencies")
                      << std::endl;
        }

        for (size_t i = 0; i < pkgs.size(); ++i) {
            std::string pkgNameAndVersion = pkgs[i];
            std::string pkg = pkgNameAndVersion;
            std::string version = "latest";

            if (pkg.find("@npm:") != std::string::npos) {
                pkg = pkg.substr(pkg.find("@npm:") + 5);
            }

            size_t atPos = std::string::npos;
            if (pkg[0] == '@') {
                atPos = pkg.find('@', 1);
            } else {
                atPos = pkg.find('@');
            }

            if (atPos != std::string::npos) {
                version = pkg.substr(atPos + 1);
                pkg = pkg.substr(0, atPos);
            }

            std::string installKey = pkg + "@" + version;
            if (stats.installedPackages.count(installKey)) {
                continue;
            }
            stats.installedPackages.insert(installKey);

            std::string treePrefix;
            for (bool last : prefixLines) {
                treePrefix += last ? "    " : "│   ";
            }
            treePrefix += (i == pkgs.size() - 1) ? "└── " : "├── ";
            std::cout << treePrefix << pkg << "@" << version << std::endl;

            std::optional<json> metadataOpt = fetchNpmMetadata(pkg, version);
            if (!metadataOpt) {
                continue;
            }
            json metadata = *metadataOpt;

            std::string resolvedVersion;
            try {
                if (version == "latest") {
                    if (!metadata.contains("dist-tags") ||
                        !metadata["dist-tags"].contains("latest")) {
                        std::cerr << "ERROR: Metadata missing dist-tags.latest for "
                                  << pkg << std::endl;
                        continue;
                    }
                    resolvedVersion = metadata["dist-tags"]["latest"];
                } else {
                    if (!metadata.contains("versions") ||
                        !metadata["versions"].contains(version)) {
                        std::cerr << "ERROR: Version " << version
                                  << " not found for " << pkg << std::endl;
                        continue;
                    }
                    resolvedVersion = version;
                }
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Exception resolving version for " << pkg
                          << ": " << e.what() << std::endl;
                continue;
            }

            if (!metadata["versions"].contains(resolvedVersion)) {
                std::cerr << "ERROR: Resolved version " << resolvedVersion
                          << " not found in metadata for " << pkg << std::endl;
                continue;
            }
            const auto& versionMeta = metadata["versions"][resolvedVersion];

            if (versionMeta.contains("deprecated")) {
                std::cout << "Deprecation Warning: \""
                          << versionMeta["deprecated"].get<std::string>() << "\" - "
                          << pkg << "@" << resolvedVersion << std::endl;
            }

            if (!versionMeta.contains("dist") || !versionMeta["dist"].contains("tarball")) {
                std::cerr << "ERROR: No tarball URL found for " << pkg << "@"
                          << resolvedVersion << std::endl;
                continue;
            }
            std::string tarballUrl = versionMeta["dist"]["tarball"];

            std::optional<std::string> tarballDataOpt = downloadTarball(tarballUrl);
            if (!tarballDataOpt) {
                continue;
            }
            std::string tarballData = *tarballDataOpt;

            try {
                fs::path nodeModulesPath = projectRootPath / "node_modules";
                fs::create_directories(nodeModulesPath);

                fs::path extractPath = nodeModulesPath / pkg;
                if (pkg.rfind('@', 0) == 0 && pkg.find('/') != std::string::npos) {
                    fs::create_directories(extractPath.parent_path());
                }

                if (tarballData.empty()) {
                    std::cerr << "ERROR: Tarball data is empty for " << pkg << std::endl;
                    continue;
                }
                extractTarball(tarballData, extractPath);
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Exception during directory creation or tarball extraction for "
                          << pkg << ": " << e.what() << std::endl;
                continue;
            }

            fs::path lockfilePath = projectRootPath / "vex.lock";
            updateLockfile(lockfilePath, pkg, resolvedVersion, versionMeta, "vex");

            if (isRoot) {
                fs::path packageJsonPath = projectRootPath / "package.json";
                if (!fs::exists(packageJsonPath)) {
                    std::cerr << "ERROR: package.json not found at "
                              << packageJsonPath.string() << std::endl;
                } else {
                    updatePackageJson(
                        packageJsonPath,
                        pkg,
                        resolvedVersion,
                        depType == DepType::DevDependencies
                    );
                    std::cout << "Added " << pkg << "@^" << resolvedVersion
                              << " to package.json" << std::endl;
                }
            }

            stats.totalUnpackedSize += versionMeta["dist"].value("unpackedSize", 0);
            stats.totalPackagesCount++;

            if (versionMeta.contains("dependencies")) {
                DepMap deps;
                for (auto const& [key, val] : versionMeta["dependencies"].items()) {
                    deps.dependencies.push_back(key + "@" + val.get<std::string>());
                }
                std::vector<bool> newPrefixLines = prefixLines;
                newPrefixLines.push_back(i == pkgs.size() - 1);
                addPackages(
                    deps,
                    false,
                    layer + 1,
                    newPrefixLines,
                    false,
                    stats,
                    projectRootPath
                );
            }
        }
        firstDepType = false;
    }

    if (isRoot) {
        std::cout << "\n\nStats" << std::endl;
        std::cout << "Size: " << stats.totalUnpackedSize << " ("
                  << (double)stats.totalUnpackedSize / (1024 * 1024) << " MB)"
                  << std::endl;
        std::cout << "Packages: " << stats.totalPackagesCount << std::endl;
    }
}

void addPackages(
    const DepMap& packages,
    bool isRoot,
    int layer,
    const std::vector<bool>& prefixLines,
    bool first,
    bool depFirst
) {
    fs::path current_dir = fs::current_path();
    fs::path project_root = current_dir;

    bool found_package_json = false;
    fs::path temp_path = current_dir;
    while (!temp_path.empty() && temp_path != temp_path.parent_path()) {
        if (fs::exists(temp_path / "package.json")) {
            project_root = temp_path;
            found_package_json = true;
            break;
        }
        temp_path = temp_path.parent_path();
    }

    if (!found_package_json) {
        std::cerr << "WARNING: package.json not found in current directory or "
                     "any parent. Assuming current directory as project root."
                  << std::endl;
    }

    PackageInstallStats stats;
    addPackages(
        packages,
        isRoot,
        layer,
        prefixLines,
        depFirst,
        stats,
        project_root
    );
}
DepMap parseAddArgs(const std::vector<std::string>& args) {
    DepMap packages;
    DepType current = DepType::Dependencies;

    for (const auto& arg : args) {
        if (arg == "--dev" || arg == "-D") {
            current = DepType::DevDependencies;
        } else if (arg == "--peer" || arg == "-P") {
            current = DepType::PeerDependencies;
        } else if (arg == "--") {
            current = DepType::Dependencies;
        } else {
            if (current == DepType::Dependencies) {
                packages.dependencies.push_back(arg);
            } else if (current == DepType::DevDependencies) {
                packages.devDependencies.push_back(arg);
            } else {
                packages.peerDependencies.push_back(arg);
            }
        }
    }
    return packages;
}