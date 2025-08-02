#include "lockfile.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

void updateLockfile(const fs::path& lockfilePath, const std::string& pkg, const std::string& version, const json& metadata, const std::string& type) {

    std::cout << "DEBUG: Using lock file path: " << lockfilePath.string() << std::endl;

    if (type == "vex") {
        json lockfile;
        if (fs::exists(lockfilePath)) {
            try {
                std::ifstream lockfile_in(lockfilePath);
                if (lockfile_in.is_open()) {
                    lockfile_in >> lockfile;
                    lockfile_in.close();
                } else {
                    std::cerr << "WARNING: Could not open existing lockfile for reading: " << lockfilePath.string() << ". Starting with empty lockfile." << std::endl;
                    lockfile = json::object();
                }
            } catch (const std::exception& e) {
                std::cerr << "Error reading " << lockfilePath.string() << ": " << e.what() << ". Starting with empty lockfile." << std::endl;
                lockfile = json::object();
            }
        } else {
            lockfile = json::object();
        }


        std::string resolvedUrl = metadata.contains("dist") && metadata["dist"].contains("tarball")
                                  ? metadata["dist"]["tarball"].get<std::string>()
                                  : "";
        std::string integrityHash = metadata.contains("dist") && metadata["dist"].contains("integrity")
                                    ? metadata["dist"]["integrity"].get<std::string>()
                                    : "";

        lockfile[pkg] = {
            {"version", version},
            {"resolved", resolvedUrl},
            {"integrity", integrityHash},
            {"dependencies", metadata.value("dependencies", json::object())}
        };

        try {
            std::ofstream lockfile_out(lockfilePath);
            if (!lockfile_out.is_open()) {
                std::cerr << "ERROR: Could not open " << lockfilePath.string() << " for writing" << std::endl;
                return;
            }

            lockfile_out << lockfile.dump(4);
            lockfile_out.close();

            if (lockfile_out.fail()) {
                std::cerr << "ERROR: Failed to write data to " << lockfilePath.string() << std::endl;
                return;
            }

            std::cout << "Updated " << lockfilePath.string() << " with " << pkg << "@" << version << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error writing " << lockfilePath.string() << ": " << e.what() << std::endl;
        }
    } else if (type == "npm") {

        json lockfile;
        if (fs::exists(lockfilePath)) {
            std::ifstream lockfile_in(lockfilePath);
            if (lockfile_in.is_open()) {
                lockfile_in >> lockfile;
                lockfile_in.close();
            } else {
                std::cerr << "WARNING: Could not open existing npm lockfile for reading: " << lockfilePath.string() << ". Starting with empty npm lockfile." << std::endl;
                lockfile = json::object();
            }
        } else {
            lockfile["packages"] = json::object();
        }
        fs::path projectRoot = lockfilePath.parent_path();
        fs::path packageJsonPath = projectRoot / "package.json";


        if (!fs::exists(packageJsonPath)) {
            std::cerr << "ERROR: package.json not found for npm lockfile update at " << packageJsonPath.string() << std::endl;
            return;
        }

        std::cout << "DEBUG: Using package.json from: " << packageJsonPath.string() << std::endl;

        json packageJson;
        try {
            std::ifstream pkg_in(packageJsonPath);
            pkg_in >> packageJson;
            pkg_in.close();
        } catch (const std::exception& e) {
            std::cerr << "Error reading package.json for npm lockfile: " << e.what() << std::endl;
            return;
        }

        lockfile["name"] = packageJson.value("name", "");
        lockfile["version"] = packageJson.value("version", "");
        lockfile["lockfileVersion"] = 3;

        lockfile["packages"][""] = {
            {"name", packageJson.value("name", "")},
            {"version", packageJson.value("version", "")},
            {"license", packageJson.value("license", "")},
            {"dependencies", packageJson.value("dependencies", json::object())},
            {"bin", packageJson.value("bin", json::object())},
            {"devDependencies", packageJson.value("devDependencies", json::object())}
        };

        std::string npm_integrity = metadata.contains("dist") && metadata["dist"].contains("integrity")
                                    ? metadata["dist"]["integrity"].get<std::string>()
                                    : "";

        lockfile["packages"]["node_modules/" + pkg] = {
            {"version", version},
            {"resolved", metadata["dist"]["tarball"]},
            {"integrity", npm_integrity},
            {"cpu", metadata.value("cpu", json::array())},
            {"license", metadata.value("license", "")},
            {"dependencies", metadata.value("dependencies", json::object())},
            {"bin", metadata.value("bin", json::object())},
            {"engines", metadata.value("engines", json::object())},
            {"funding", metadata.value("funding", json::object())},
            {"optionalDependencies", metadata.value("optionalDependencies", json::object())},
            {"peerDependencies", metadata.value("peerDependencies", json::object())},
            {"peerDependenciesMeta", metadata.value("peerDependenciesMeta", json::object())}
        };

        try {
            std::ofstream lockfile_out(lockfilePath);
            if (!lockfile_out.is_open()) {
                std::cerr << "ERROR: Could not open " << lockfilePath.string() << " for writing (npm)" << std::endl;
                return;
            }
            lockfile_out << lockfile.dump(4);
            lockfile_out.close();
            if (lockfile_out.fail()) {
                std::cerr << "ERROR: Failed to write data to " << lockfilePath.string() << " (npm)" << std::endl;
                return;
            }
            std::cout << "Updated " << lockfilePath.string() << " with " << pkg << "@" << version << " (npm format)" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error writing " << lockfilePath.string() << " (npm): " << e.what() << std::endl;
        }
    }
}