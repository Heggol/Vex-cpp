#include "arguments.h"

ParsedArgs parseArguments(int argc, char** argv) {
    ParsedArgs parsedArgs;

    if (argc < 2) {
        parsedArgs.command = Command::UNKNOWN;
        return parsedArgs;
    }

    std::string commandStr = argv[1];
    if (commandStr == "add") {
        parsedArgs.command = Command::ADD;
    } else if (commandStr == "init") {
        parsedArgs.command = Command::INIT;
    } else if (commandStr == "install") {
        parsedArgs.command = Command::INSTALL;
    } else if (commandStr == "remove") {
        parsedArgs.command = Command::REMOVE;
    } else if (commandStr == "help") {
        parsedArgs.command = Command::HELP;
    } else {
        parsedArgs.command = Command::UNKNOWN;
        parsedArgs.values.push_back(commandStr);
        return parsedArgs;
    }

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (parsedArgs.command == Command::INIT && (arg == "-y" || arg == "--yes")) {
            parsedArgs.yes_flag = true;
        } else {
            parsedArgs.values.push_back(arg);
        }
    }

    return parsedArgs;
}