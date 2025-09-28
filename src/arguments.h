#pragma once
#include <string>
#include <vector>

enum class Command {
    ADD,
    INIT,
    INSTALL,
    REMOVE,
    HELP,
    UNKNOWN
};

struct ParsedArgs {
    Command command = Command::UNKNOWN;
    std::vector<std::string> values;
    bool yes_flag = false; // For init command
};

ParsedArgs parseArguments(int argc, char** argv);