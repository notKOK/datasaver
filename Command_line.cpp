#include "architecture.h"
#include "iostream"

using namespace std;

Command_line::Command_line(int argc, char* argv[])
{
    desc_.add_options()
            ("help,h", "produce help message")
            ("DataFile,f", po::value<string>(), "set DataFile name")
            ("dir,d", po::value<string>(), "set directory path")
            ("search,s", po::value<string>(), "search for a file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc_), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc_ << endl;
        exit(0);
    }

    if (vm.count("DataFile")) {
        filename_ = vm["DataFile"].as<string>();
    }

    if (vm.count("dir")) {
        directory_ = vm["dir"].as<string>();
    }

    if (vm.count("search")) {
        search_param = vm["search"].as<string>();
    }
    f_srch = true;
}

string Command_line::run()
{
    if (!filename_.empty()) {
        f_srch = true;
        return filename_;
    }

    if (!directory_.empty()) {
        cout << "Directory path: " << directory_ << endl;
        return directory_;
    }

    if (!search_param.empty()) {
        f_srch = false;
        cout << "Search: " << search_param << endl;
        return search_param;
    }
    cout << "Search: " << search_param << endl;
    cout << "directory_: " << directory_ << endl;
    cout << "filename_: " << filename_ << endl;
    cout << "f_srch: " << f_srch << endl;
    cout << "desc_: " << desc_ << endl;

    return "";
}

Command_line::~Command_line() = default;
