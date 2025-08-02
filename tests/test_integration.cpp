#include <gtest/gtest.h>
#include "../src/commands/add.h"
#include "../src/commands/install.h"
#include "../src/commands/init.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <chrono>
#include <sstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

class IntegrationTestF : public ::testing::Test {
protected:
    fs::path original_path;
    fs::path test_dir;

    void SetUp() override {
        original_path = fs::current_path();
        const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string test_name = test_info->name();
        auto timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::stringstream ss;
        ss << "vex_"<< test_name << "_" << timestamp;
        test_dir = fs::temp_directory_path() / ss.str();
        
        fs::create_directory(test_dir);
        fs::current_path(test_dir);

        init(true);
    }

    void TearDown() override {
        fs::current_path(original_path);
        fs::remove_all(test_dir);
    }
};

TEST_F(IntegrationTestF, Add_InstallsPackageAndUpdatesFiles) {
    DepMap deps;
    deps.dependencies.push_back("is-odd@3.0.1");
    
    addPackages(deps);

    ASSERT_TRUE(fs::exists("node_modules/is-odd")) << "Package directory should exist in node_modules.";
    ASSERT_TRUE(fs::exists("node_modules/is-odd/package.json")) << "package.json should exist inside the package directory.";

    std::ifstream pkg_in("package.json");
    ASSERT_TRUE(pkg_in.is_open());
    json pkg_json;
    pkg_in >> pkg_json;
    ASSERT_TRUE(pkg_json.contains("dependencies"));
    ASSERT_TRUE(pkg_json["dependencies"].contains("is-odd"));
    EXPECT_EQ(pkg_json["dependencies"]["is-odd"], "^3.0.1");

    std::ifstream lock_in("vex.lock");
    ASSERT_TRUE(lock_in.is_open());
    json lock_json;
    lock_in >> lock_json;
    ASSERT_TRUE(lock_json.contains("is-odd"));
    EXPECT_EQ(lock_json["is-odd"]["version"], "3.0.1");
}

