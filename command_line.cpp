#include "architecture.h"
#include "iostream"

using namespace std;

command_line::command_line(int argc, char* argv[])
{
    desc_.add_options()
            ("help,h", "produce help message")
            ("dataFile,f", po::value<string>(), "set dataFile name")
            ("dir,d", po::value<string>(), "set directory path")
            ("search,s", po::value<string>(), "search for a file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc_), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc_ << endl;
        exit(0);
    }

    if (vm.count("dataFile")) {
        filename_ = vm["dataFile"].as<string>();
    }

    if (vm.count("dir")) {
        directory_ = vm["dir"].as<string>();
    }

    if (vm.count("search")) {
        search_param = vm["search"].as<string>();
    }
}

string command_line::run()
{
    if (!filename_.empty()) {
        f_srch = true;
        return filename_;
    }

    if (!directory_.empty()) {
        cout << "Directory path: " << directory_ << endl;
        exit(0);
        return directory_;
    }

    if (!search_param.empty()) {
        f_srch = false;
        cout << "Search result: " << search_param << endl;
        return search_param;
    }
    //exit(1);
    return "";
}