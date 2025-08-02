#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace fs = std::filesystem;

using json = nlohmann::json;

void updateLockfile(const fs::path& lockfilePath, const std::string& packageName, const std::string& version, const nlohmann::json& versionMeta, const std::string& installer);
