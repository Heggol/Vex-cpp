#include <iostream>
#include <filesystem>
#include "arguments.h"
#include "commands/add.h"
#include "commands/init.h"
#include "commands/install.h"
#include "commands/remove.h"
#include "commands/help.h"
#include "utils/windows_setup.h"

namespace fs = std::filesystem;

void printUsage() {
    std::cout << "Usage: vex <command> [options]\n"
              << "Commands:\n"
              << "  add <packages ...>     Add packages to the project\n"
              << "  init [-y|--yes]        Initialize a new project\n"
              << "  install                Install project dependencies\n"
              << "  remove <package>       Remove a package from the project\n"
              << "  help <command>         Display help for a command\n";
}

int main(int argc, char** argv) {
    setupConsole();
    ParsedArgs args = parseArguments(argc, argv);

    switch (args.command)
    {
    case Command::ADD:
        if (args.values.empty()) {
            std::cerr << "Error: No packages specified for 'add' command.\n";
            printUsage();
            return 1;
        }
        addPackages(parseAddArgs(args.values));
        installPackages();
        break;
    case Command::INIT:
        init(args.yes_flag);
        break;
    case Command::INSTALL:
        installPackages();
        break;
    case Command::REMOVE:
        if (args.values.size() != 1) {
            std::cerr << "Error: 'remove' command requires exactly one package name.\n";
            printUsage();
            return 1;
        }
        removePackage(args.values[0]);
        installPackages();
        break;
    case Command::HELP:
        help(args.values.empty() ? "" : args.values[0]);
        break;
    case Command::UNKNOWN:
    default:
            if (!args.values.empty()) {
                std::cerr << "Unknown command: " << args.values[0] << std::endl;
            }
            printUsage();
            return 1;
    }

    return 0;
}