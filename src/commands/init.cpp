#include "init.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

std::string askQuestion(const std::string& query, const std::string& defaultValue = "") {
    std::cout << query;
    if (!defaultValue.empty()) {
        std::cout << " (" << defaultValue << ")";
    }
    std::cout << ": ";
    std::string answer;
    std::getline(std::cin, answer);
    if (answer.empty()) {
        return defaultValue;
    }
    return answer;
}

json defaultPackageJson(const std::string& name) {
    return {
        {"name", name},
        {"version", "1.0.0"},
        {"description", ""},
        {"main", "index.js"},
        {"scripts", {
            {"test", "echo \"Error: no test specified\" && exit 1"}
        }},
        {"keywords", json::array()},
        {"author", ""},
        {"license", "ISC"}
    };
}

void init(bool yes) {
    if (fs::exists("package.json")) {
        std::cout << "A package.json already exists at this location." << std::endl;
        std::string answer = askQuestion("Do you wish to override the existing package.json? (y/N)");
        if (answer != "y" && answer != "Y") {
            return;
        }
    }

    json pkg;
    std::string dirName = fs::current_path().filename().string();

    if (yes) {
        pkg = defaultPackageJson(dirName);
    } else {
        std::string name = askQuestion("Package name", dirName);
        std::string version = askQuestion("Version", "1.0.0");
        std::string desc = askQuestion("Description");
        std::string main = askQuestion("Entry point", "index.js");
        std::string testScript = askQuestion("Test script");
        std::string author = askQuestion("Author");
        std::string license = askQuestion("License", "ISC");

        pkg = {
            {"name", name},
            {"version", version},
            {"description", desc},
            {"main", main},
            {"scripts", {
                {"test", testScript}
            }},
            {"keywords", json::array()},
            {"author", author},
            {"license", license}
        };
    }

    std::ofstream file("package.json");
    file << pkg.dump(4);
    file.close();

    std::cout << "Successfully created package.json" << std::endl;
    std::cout << pkg.dump(4) << std::endl;
}
