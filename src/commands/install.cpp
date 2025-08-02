#include "install.h"
#include "add.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <httplib/httplib.h>

namespace fs = std::filesystem;
using json = nlohmann::json;

void extractTarball(const std::string& buffer, const fs::path& extractPath);

void installPackages() {
    fs::path lockPath = "vex.lock";
    if (!fs::exists(lockPath)) {
        fs::path npmLockPath = "package-lock.json";
        if (!fs::exists(npmLockPath)) {
            std::cerr << "No vex.lock or package-lock.json file found. Run 'vex add <package>' to create one." << std::endl;
            return;
        }
        lockPath = npmLockPath;
    }

    json lockfile;
    std::ifstream lockfile_in(lockPath);
    try {
        lockfile_in >> lockfile;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing " << lockPath << ": " << e.what() << std::endl;
        return;
    }
    lockfile_in.close();

    if (lockPath == "vex.lock") {
        if (lockfile.empty()) {
            std::cout << "Nothing to install. Lock file is empty." << std::endl;
            return;
        }
        std::cout << "Installing packages from vex.lock..." << std::endl;
    } else if (!lockfile.contains("packages") || lockfile["packages"].empty()) {
        std::cout << "Nothing to install. No packages in lock file." << std::endl;
        return;
    } else {
        std::cout << "Installing packages from package-lock.json..." << std::endl;
    }

    fs::create_directory("node_modules");

    if (lockPath == "vex.lock") {
        for (auto& [pkgName, meta] : lockfile.items()) {
            fs::path extractPath = fs::path("node_modules") / pkgName;
            
            std::string tarballUrl = meta["resolved"];
            std::string host = tarballUrl.substr(8, tarballUrl.find('/', 8) - 8);
            std::string path = tarballUrl.substr(tarballUrl.find('/', 8));

            httplib::Client tar_cli(host.c_str());
            auto tar_res = tar_cli.Get(path.c_str());

            if (tar_res && tar_res->status == 200) {
                extractTarball(tar_res->body, extractPath);
                std::cout << "Installed " << pkgName << "@" << meta["version"].get<std::string>() << std::endl;
            }
        }
    } else {
        for (auto& [pkgPath, meta] : lockfile["packages"].items()) {
            if (pkgPath.empty()) continue;

            std::string pkgName = pkgPath.substr(pkgPath.find('/') + 1);
            
            fs::path extractPath = fs::path("node_modules") / pkgName;
            
            std::string tarballUrl = meta["resolved"];
            std::string host = tarballUrl.substr(8, tarballUrl.find('/', 8) - 8);
            std::string path = tarballUrl.substr(tarballUrl.find('/', 8));

            httplib::Client tar_cli(host.c_str());
            auto tar_res = tar_cli.Get(path.c_str());

            if (tar_res && tar_res->status == 200) {
                extractTarball(tar_res->body, extractPath);
                std::cout << "Installed " << pkgName << "@" << meta["version"].get<std::string>() << std::endl;
            } else {
                std::cerr << "Failed to download tarball for " << pkgName << std::endl;
            }
        }
    }

    std::cout << "All packages installed successfully." << std::endl;
}
