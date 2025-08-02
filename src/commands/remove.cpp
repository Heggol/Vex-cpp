#include "remove.h"
#include "../package.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void removePackage(const std::string& packageName, bool dev) {
    fs::path modulesPath = "node_modules";
    fs::path packagePath = modulesPath / packageName;

    try {
        if (fs::exists(packagePath)) {
            fs::remove_all(packagePath);
        }

        fs::path packageJsonPath = "package.json";
        removePackageJson(packageJsonPath, packageName, dev);

        fs::path lockfilePath = "package-lock.json";
        if (fs::exists(lockfilePath)) {
            json lockfile;
            std::ifstream lockfile_in(lockfilePath);
            lockfile_in >> lockfile;
            lockfile_in.close();

            std::string nodeModulesPkg = "node_modules/" + packageName;
            if (lockfile["packages"].contains(nodeModulesPkg)) {
                lockfile["packages"].erase(nodeModulesPkg);
                std::ofstream lockfile_out(lockfilePath);
                lockfile_out << lockfile.dump(4);
                lockfile_out.close();
            }
        }

        std::cout << "Removed " << packageName << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to remove " << packageName << ": " << e.what() << std::endl;
    }
}
