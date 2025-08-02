#include <gtest/gtest.h>
#include "../src/commands/init.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <chrono>
#include <sstream>

using json = nlohmann::json;
namespace fs = std::filesystem;

class InitTestF : public ::testing::Test {
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
};

TEST_F(InitTestF, InitYes_CreatesPackageJsonWithDefaults) {
    init(true);

    fs::path pkg_path = "package.json";
    ASSERT_TRUE(fs::exists(pkg_path));

    std::ifstream f(pkg_path);
    ASSERT_TRUE(f.is_open());
    json data;
    f >> data;

    EXPECT_EQ(data["name"], test_dir.filename().string());
    EXPECT_EQ(data["version"], "1.0.0");
    EXPECT_EQ(data["description"], "");
    EXPECT_EQ(data["main"], "index.js");
    ASSERT_TRUE(data.contains("scripts"));
    EXPECT_EQ(data["scripts"]["test"], "echo \"Error: no test specified\" && exit 1");
    ASSERT_TRUE(data["keywords"].is_array());
    EXPECT_EQ(data["keywords"].size(), 0);
    EXPECT_EQ(data["author"], "");
    EXPECT_EQ(data["license"], "ISC");
}