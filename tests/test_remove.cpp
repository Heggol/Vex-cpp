#include <gtest/gtest.h>
#include "../src/commands/remove.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <chrono>
#include <sstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

class RemoveTestF : public ::testing::Test {
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
    }

    void TearDown() override {
        fs::current_path(original_path);
        fs::remove_all(test_dir);
    }

    void setupProjectWithDependency() {
        json pkg_json = {
            {"dependencies", {
                {"is-odd", "3.0.1"}
            }},
            {"devDependencies", {
                {"is-even", "1.0.0"}
            }}
        };
        std::ofstream out_pkg("package.json");
        out_pkg << pkg_json.dump(4);
        out_pkg.close();

        json lock_json = {
            {"packages", {
                {"node_modules/is-odd", {
                    {"version", "3.0.1"}
                }},
                {"node_modules/is-even", {
                    {"version", "1.0.0"}
                }}
            }}
        };
        std::ofstream out_lock("package-lock.json");
        out_lock << lock_json.dump(4);
        out_lock.close();

        fs::create_directories("node_modules/is-odd");
        std::ofstream out_is_odd("node_modules/is-odd/index.js");
        out_is_odd << "// dummy file";
        out_is_odd.close();
        
        fs::create_directories("node_modules/is-even");
        std::ofstream out_is_even("node_modules/is-even/index.js");
        out_is_even << "// dummy file";
        out_is_even.close();
    }
};

TEST_F(RemoveTestF, RemovePackageFromDependencies) {
    setupProjectWithDependency();

    ASSERT_TRUE(fs::exists("node_modules/is-odd"));
    ASSERT_TRUE(fs::exists("package.json"));
    ASSERT_TRUE(fs::exists("package-lock.json"));
    
    removePackage("is-odd", false);

    EXPECT_FALSE(fs::exists("node_modules/is-odd"));
    EXPECT_TRUE(fs::exists("node_modules/is-even"));

    std::ifstream in_pkg("package.json");
    json pkg_after;
    in_pkg >> pkg_after;
    in_pkg.close();
    EXPECT_FALSE(pkg_after["dependencies"].contains("is-odd"));
    EXPECT_TRUE(pkg_after["devDependencies"].contains("is-even"));

    std::ifstream in_lock("package-lock.json");
    json lock_after;
    in_lock >> lock_after;
    in_lock.close();
    EXPECT_FALSE(lock_after["packages"].contains("node_modules/is-odd"));
    EXPECT_TRUE(lock_after["packages"].contains("node_modules/is-even"));
}

TEST_F(RemoveTestF, RemovePackageFromDevDependencies) {
    setupProjectWithDependency();

    ASSERT_TRUE(fs::exists("node_modules/is-even"));
    
    removePackage("is-even", true);

    EXPECT_FALSE(fs::exists("node_modules/is-even"));
    EXPECT_TRUE(fs::exists("node_modules/is-odd"));

    std::ifstream in_pkg("package.json");
    json pkg_after;
    in_pkg >> pkg_after;
    in_pkg.close();
    EXPECT_FALSE(pkg_after["devDependencies"].contains("is-even"));
    EXPECT_TRUE(pkg_after["dependencies"].contains("is-odd"));

    std::ifstream in_lock("package-lock.json");
    json lock_after;
    in_lock >> lock_after;
    in_lock.close();
    EXPECT_FALSE(lock_after["packages"].contains("node_modules/is-even"));
    EXPECT_TRUE(lock_after["packages"].contains("node_modules/is-odd"));
}