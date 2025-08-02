#include "help.h"
#include <iostream>

void help(const std::string& command) {
    if (command.empty()) {
        std::cout << "Usage: vex <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  add <package>      Add a package to the project" << std::endl;
        std::cout << "  remove <package>   Remove a package from the project" << std::endl;
        std::cout << "  install            Install project dependencies" << std::endl;
        std::cout << "  init               Initialize a new project" << std::endl;
        std::cout << "  help <command>     Display help for a command" << std::endl;
    } else if (command == "add") {
        std::cout << "Usage: vex add <package>" << std::endl;
        std::cout << "Adds a package to the project." << std::endl;
    } else if (command == "remove") {
        std::cout << "Usage: vex remove <package>" << std::endl;
        std::cout << "Removes a package from the project." << std::endl;
    } else if (command == "install") {
        std::cout << "Usage: vex install" << std::endl;
        std::cout << "Installs project dependencies from the lockfile." << std::endl;
    } else if (command == "init") {
        std::cout << "Usage: vex init" << std::endl;
        std::cout << "Initializes a new project, creating a package.json file." << std::endl;
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Run 'vex help' for a list of commands." << std::endl;
    }
}
