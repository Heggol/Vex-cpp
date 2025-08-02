#include <gtest/gtest.h>
#include "../src/commands/install.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

class InstallTestF : public ::testing::Test {
protected:
    fs::path original_path;
    fs::path test_dir;
    std::streambuf* old_cout;
    std::stringstream cout_buffer;

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

        old_cout = std::cout.rdbuf(cout_buffer.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(old_cout);
        fs::current_path(original_path);
        fs::remove_all(test_dir);
    }
};

TEST_F(InstallTestF, Install_EmptyVexLock) {
    std::ofstream out("vex.lock");
    out << "{}";
    out.close();

    installPackages();
    std::string output = cout_buffer.str();
    EXPECT_NE(output.find("Nothing to install. Lock file is empty."), std::string::npos);
}

TEST_F(InstallTestF, Install_EmptyPackageLockJson) {
    json lock_json = {
        {"name", "test-project"},
        {"version", "1.0.0"},
        {"lockfileVersion", 3},
        {"packages", {}}
    };
    std::ofstream out("package-lock.json");
    out << lock_json.dump(4);
    out.close();

    installPackages();
    std::string output = cout_buffer.str();
    EXPECT_NE(output.find("Nothing to install. No packages in lock file."), std::string::npos);
}
