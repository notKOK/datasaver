#include <string>
#include <boost/program_options.hpp>

#ifndef DATASAVER_ARCHITECTURE_H
#define DATASAVER_ARCHITECTURE_H

namespace po = boost::program_options;

class command_line
{
public:
    command_line(int argc, char* argv[]);
    int run();

private:
    po::options_description desc_;
    std::string filename_;
    std::string directory_;
};

class file; //datagramm or data file;
class database_connect;






#endif //DATASAVER_ARCHITECTURE_H
