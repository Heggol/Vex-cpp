#include "package.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;


void updatePackageJson(
    const fs::path& packageJsonPath,
    const std::string& pkg,
    const std::string& version,
    bool dev
) {
    std::cout << "DEBUG: Attempting to update package.json at: " << packageJsonPath.string() << std::endl;

    if (!fs::exists(packageJsonPath)) {
        std::cerr << "ERROR: package.json not found at specified path: " << packageJsonPath.string() << std::endl;
        return;
    }

    json packageJson;
    try {
        std::ifstream pkg_in(packageJsonPath);
        if (!pkg_in.is_open()) {
            std::cerr << "ERROR: Could not open package.json for reading: " << packageJsonPath.string() << std::endl;
            return;
        }
        pkg_in >> packageJson;
        pkg_in.close();
        std::cout << "DEBUG: Successfully read package.json" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to parse package.json from " << packageJsonPath.string() << ": " << e.what() << std::endl;
        return;
    }

    std::string depType = dev ? "devDependencies" : "dependencies";
    std::cout << "DEBUG: Adding to " << depType << " section" << std::endl;

    if (!packageJson.contains(depType) || !packageJson[depType].is_object()) {
        std::cout << "DEBUG: Creating " << depType << " section in package.json" << std::endl;
        packageJson[depType] = json::object();
    }

    std::cout << "DEBUG: Setting " << pkg << " to ^" << version << " in " << depType << std::endl;
    packageJson[depType][pkg] = "^" + version;

    try {
        std::ofstream pkg_out(packageJsonPath, std::ios::out | std::ios::trunc);
        if (!pkg_out.is_open()) {
            std::cerr << "ERROR: Could not open package.json for writing. Check permissions for: " << packageJsonPath.string() << std::endl;
            return;
        }

        std::string jsonContent = packageJson.dump(4);
        pkg_out << jsonContent;
        pkg_out.close();

        if (pkg_out.fail()) {
            std::cerr << "ERROR: Failed to write data to package.json: " << packageJsonPath.string() << std::endl;
            return;
        }

        std::cout << "Successfully updated package.json with " << pkg << "@^" << version << std::endl;

        std::ifstream verify(packageJsonPath);
        if (verify.good()) {
            std::string content((std::istreambuf_iterator<char>(verify)), std::istreambuf_iterator<char>());
            verify.close();
            if (content.find(pkg) != std::string::npos) {
                std::cout << "DEBUG: Verified package.json contains " << pkg << std::endl;
            } else {
                std::cerr << "ERROR: Verification failed: package.json exists but does not contain " << pkg << std::endl;
            }
        } else {
            std::cerr << "ERROR: Could not open package.json for verification after write." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception writing package.json to " << packageJsonPath.string() << ": " << e.what() << std::endl;
    }
}

void removePackageJson(const fs::path& packageJsonPath, const std::string& pkg, bool dev) {
    std::cout << "DEBUG: Attempting to remove " << pkg << " from package.json at: " << packageJsonPath.string() << std::endl;

    if (!fs::exists(packageJsonPath)) {
        std::cerr << "ERROR: package.json not found at specified path for removal: " << packageJsonPath.string() << std::endl;
        return;
    }

    json packageJson;
    try {
        std::ifstream pkg_in(packageJsonPath);
        if (!pkg_in.is_open()) {
            std::cerr << "ERROR: Could not open package.json for reading during removal: " << packageJsonPath.string() << std::endl;
            return;
        }
        pkg_in >> packageJson;
        pkg_in.close();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to parse package.json for removal from " << packageJsonPath.string() << ": " << e.what() << std::endl;
        return;
    }

    std::string depType = dev ? "devDependencies" : "dependencies";

    if (packageJson.contains(depType) && packageJson[depType].is_object() && packageJson[depType].contains(pkg)) {
        packageJson[depType].erase(pkg);
        std::cout << "DEBUG: Erased " << pkg << " from " << depType << std::endl;
    } else {
        std::cout << "DEBUG: Package " << pkg << " not found in " << depType << " or section does not exist." << std::endl;
    }

    try {
        std::ofstream pkg_out(packageJsonPath, std::ios::out | std::ios::trunc);
        if (!pkg_out.is_open()) {
            std::cerr << "ERROR: Could not open package.json for writing during removal. Check permissions for: " << packageJsonPath.string() << std::endl;
            return;
        }
        pkg_out << packageJson.dump(4);
        pkg_out.close();

        if (pkg_out.fail()) {
            std::cerr << "ERROR: Failed to write data to package.json after removal: " << packageJsonPath.string() << std::endl;
            return;
        }
        std::cout << "Successfully removed " << pkg << " from package.json" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception writing package.json after removal to " << packageJsonPath.string() << ": " << e.what() << std::endl;
    }
}