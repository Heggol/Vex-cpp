#pragma once
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class DepType {
    Dependencies,
    DevDependencies,
    PeerDependencies
};

struct DepMap {
    std::vector<std::string> dependencies;
    std::vector<std::string> devDependencies;
    std::vector<std::string> peerDependencies;
};

void addPackages(const DepMap& packages, bool isRoot = true, int layer = 0, const std::vector<bool>& prefixLines = {}, bool first = true, bool depFirst = true);
DepMap parseAddArgs(const std::vector<std::string>& args);
