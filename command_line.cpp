#include "architecture.h"
#include "iostream"

command_line::command_line(int argc, char* argv[])
{
    desc_.add_options()
            ("help,h", "produce help message")
            ("file,f", po::value<std::string>(), "set file name")
            ("dir,d", po::value<std::string>(), "set directory path");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc_), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc_ << std::endl;
        exit(0);
    }

    if (vm.count("file")) {
        filename_ = vm["file"].as<std::string>();
    }

    if (vm.count("dir")) {
        directory_ = vm["dir"].as<std::string>();
    }
}

int command_line::run()
{
    if (!filename_.empty()) {
        std::cout << "File name: " << filename_ << std::endl;
    }

    if (!directory_.empty()) {
        std::cout << "Directory path: " << directory_ << std::endl;
    }

    // ваш код

    return 0;
}