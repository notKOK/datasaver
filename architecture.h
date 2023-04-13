#include <string>
#include <boost/program_options.hpp>
#include <fstream>
#include "structs.h"

#ifndef DATASAVER_ARCHITECTURE_H
#define DATASAVER_ARCHITECTURE_H

namespace po = boost::program_options;

class command_line
{
public:
    command_line(int argc, char* argv[]);
    std::string run();

private:
    po::options_description desc_;
    std::string filename_;
    std::string directory_;
};

class dataFile {
public:
    explicit dataFile(const std::string& file_name);
    ~dataFile();
    void readDataFromFile();
    subheading davot();

private:
    std::ifstream datFile;
    stringStructs firstString;
    stringStructs secondString;
    subheading subheader{};
    locator_operation locatorOperation{};
    receiver recev{};
    transmitter transmt{};
    synchronizer synch{};
    generator gen{};
    JSO jso{};
    antenna_system antennaSystem{};
    ACP acp{};
    format_string formatString{};
}; //datagramm or data dataFile
class database_connect;






#endif //DATASAVER_ARCHITECTURE_H
