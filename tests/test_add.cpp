#include <gtest/gtest.h>
#include "../src/commands/add.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using json = nlohmann::json;

TEST(AddPackagesTest, ParseAddArgsBasic) {
    std::vector<std::string> args = {"pkg1", "pkg2"};
    DepMap result = parseAddArgs(args);
    EXPECT_EQ(result.dependencies.size(), 2);
    EXPECT_EQ(result.devDependencies.size(), 0);
    EXPECT_EQ(result.peerDependencies.size(), 0);
    EXPECT_EQ(result.dependencies[0], "pkg1");
    EXPECT_EQ(result.dependencies[1], "pkg2");
}

TEST(AddPackagesTest, ParseAddArgsDevFlag) {
    std::vector<std::string> args = {"--dev", "pkg1", "pkg2"};
    DepMap result = parseAddArgs(args);
    EXPECT_EQ(result.dependencies.size(), 0);
    EXPECT_EQ(result.devDependencies.size(), 2);
    EXPECT_EQ(result.peerDependencies.size(), 0);
    EXPECT_EQ(result.devDependencies[0], "pkg1");
    EXPECT_EQ(result.devDependencies[1], "pkg2");
}

TEST(AddPackagesTest, ParseAddArgsDevFlagShort) {
    std::vector<std::string> args = {"-D", "pkg1"};
    DepMap result = parseAddArgs(args);
    EXPECT_EQ(result.dependencies.size(), 0);
    EXPECT_EQ(result.devDependencies.size(), 1);
    EXPECT_EQ(result.peerDependencies.size(), 0);
    EXPECT_EQ(result.devDependencies[0], "pkg1");
}

TEST(AddPackagesTest, ParseAddArgsPeerFlag) {
    std::vector<std::string> args = {"--peer", "pkg1"};
    DepMap result = parseAddArgs(args);
    EXPECT_EQ(result.dependencies.size(), 0);
    EXPECT_EQ(result.devDependencies.size(), 0);
    EXPECT_EQ(result.peerDependencies.size(), 1);
    EXPECT_EQ(result.peerDependencies[0], "pkg1");
}

TEST(AddPackagesTest, ParseAddArgsPeerFlagShort) {
    std::vector<std::string> args = {"-P", "pkg1"};
    DepMap result = parseAddArgs(args);
    EXPECT_EQ(result.dependencies.size(), 0);
    EXPECT_EQ(result.devDependencies.size(), 0);
    EXPECT_EQ(result.peerDependencies.size(), 1);
    EXPECT_EQ(result.peerDependencies[0], "pkg1");
}

TEST(AddPackagesTest, ParseAddArgsMixed) {
    std::vector<std::string> args = {"pkg1", "--dev", "pkg2", "--peer", "pkg3", "pkg4"};
    DepMap result = parseAddArgs(args);
    EXPECT_EQ(result.dependencies.size(), 1);
    EXPECT_EQ(result.devDependencies.size(), 1);
    EXPECT_EQ(result.peerDependencies.size(), 2);
    EXPECT_EQ(result.dependencies[0], "pkg1");
    EXPECT_EQ(result.devDependencies[0], "pkg2");
    EXPECT_EQ(result.peerDependencies[0], "pkg3");
    EXPECT_EQ(result.peerDependencies[1], "pkg4");
}

TEST(AddPackagesTest, ParseAddArgsSeparator) {
    std::vector<std::string> args = {"--dev", "pkg1", "--", "pkg2"};
    DepMap result = parseAddArgs(args);
    EXPECT_EQ(result.dependencies.size(), 1);
    EXPECT_EQ(result.devDependencies.size(), 1);
    EXPECT_EQ(result.peerDependencies.size(), 0);
    EXPECT_EQ(result.devDependencies[0], "pkg1");
    EXPECT_EQ(result.dependencies[0], "pkg2");
}