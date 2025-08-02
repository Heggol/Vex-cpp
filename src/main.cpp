#include <iostream>
#include <filesystem>
#include "CLI/CLI.hpp"
#include "commands/add.h"
#include "commands/init.h"
#include "commands/install.h"
#include "commands/remove.h"
#include "commands/help.h"
#include "utils/windows_setup.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    setup_console();
    CLI::App app{"vex"};

    std::vector<std::string> add_pkgs;
    auto add_cmd = app.add_subcommand("add", "Add a package to the project");
    add_cmd->add_option("packages", add_pkgs, "Packages to add")->required();

    std::vector<std::string> init_args;
    auto init_cmd = app.add_subcommand("init", "Initialize a new project");
    bool init_yes = false;
    init_cmd->add_flag("-y,--yes", init_yes, "Automatically answer yes to all prompts");

    auto install_cmd = app.add_subcommand("install", "Install project dependencies");
    
    std::string remove_pkg;
    auto remove_cmd = app.add_subcommand("remove", "Remove a package from the project");
    remove_cmd->add_option("package", remove_pkg, "Package to remove")->required();

    std::string help_cmd_arg;
    auto help_cmd = app.add_subcommand("help", "Display help for a command");
    help_cmd->add_option("command", help_cmd_arg, "Command to get help for");

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);

    if (app.get_subcommand("add")->parsed()) {
        std::cout << "DEBUG: 'add' command detected with args:";
        for (const auto& pkg : add_pkgs) {
            std::cout << " " << pkg;
        }
        std::cout << std::endl;
        
        auto packages = parseAddArgs(add_pkgs);
        std::cout << "DEBUG: Dependencies count: " << packages.dependencies.size() 
                  << ", DevDependencies: " << packages.devDependencies.size()
                  << ", PeerDependencies: " << packages.peerDependencies.size() << std::endl;
        std::cout << "DEBUG: Current working directory: " << fs::current_path().string() << std::endl;
        
        addPackages(packages);
        std::cout << "Packages added to package.json. Installing..." << std::endl;
        installPackages();
    } else if (app.get_subcommand("init")->parsed()) {
        init(init_yes);
    } else if (app.get_subcommand("install")->parsed()) {
        installPackages();
    } else if (app.get_subcommand("remove")->parsed()) {
        removePackage(remove_pkg);
    } else if (app.get_subcommand("help")->parsed()) {
        help(help_cmd_arg);
    }

    return 0;
}