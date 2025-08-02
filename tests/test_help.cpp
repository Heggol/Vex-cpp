#include <gtest/gtest.h>
#include "../src/commands/help.h"
#include <sstream>
#include <iostream>

class HelpTestF : public ::testing::Test {
protected:
    std::streambuf* old_cout;
    std::stringstream cout_buffer;

    void SetUp() override {
        old_cout = std::cout.rdbuf(cout_buffer.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(old_cout);
    }

    std::string getOutput() {
        return cout_buffer.str();
    }
};

TEST_F(HelpTestF, GeneralHelp) {
    help("");
    std::string output = getOutput();
    EXPECT_NE(output.find("Usage: vex <command> [options]"), std::string::npos);
    EXPECT_NE(output.find("Commands:"), std::string::npos);
    EXPECT_NE(output.find("add"), std::string::npos);
    EXPECT_NE(output.find("remove"), std::string::npos);
    EXPECT_NE(output.find("install"), std::string::npos);
    EXPECT_NE(output.find("init"), std::string::npos);
}

TEST_F(HelpTestF, AddHelp) {
    help("add");
    std::string output = getOutput();
    EXPECT_NE(output.find("Usage: vex add <package>"), std::string::npos);
}

TEST_F(HelpTestF, RemoveHelp) {
    help("remove");
    std::string output = getOutput();
    EXPECT_NE(output.find("Usage: vex remove <package>"), std::string::npos);
}

TEST_F(HelpTestF, InstallHelp) {
    help("install");
    std::string output = getOutput();
    EXPECT_NE(output.find("Usage: vex install"), std::string::npos);
}

TEST_F(HelpTestF, InitHelp) {
    help("init");
    std::string output = getOutput();
    EXPECT_NE(output.find("Usage: vex init"), std::string::npos);
}

TEST_F(HelpTestF, UnknownCommandHelp) {
    help("nonexistent");
    std::string output = getOutput();
    EXPECT_NE(output.find("Unknown command: nonexistent"), std::string::npos);
}