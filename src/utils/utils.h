#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void extractTarball(const std::string& tarballData, const fs::path& extractPath);
void createDirectory(const std::string& path);

