#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace fs = std::filesystem;

using json = nlohmann::json;

void updatePackageJson(const fs::path& packageJsonPath, const std::string& packageName, const std::string& version, bool isDevDependency);
void removePackageJson(const fs::path& packageJsonPath, const std::string& pkg, bool dev);