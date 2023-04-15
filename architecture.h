#include <string>
#include <boost/program_options.hpp>
#include <fstream>
#include "structs.h"
#include <pqxx/pqxx>


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
    void readHeaderFromFile();
    void readStringsFromFile();
    std::vector<std::string> createQuery();

private:
    std::ifstream datFile;

    stringStructs firstString;
    stringStructs lastString;

    void readOneString(stringStructs *oneString);
    std::string createQueryIntoFlights();
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
};

class database{
public:
    database(const std::string& connString);
    ~database();
    void execute(const std::vector<std::string>& sqlStatements);
private:
    pqxx::connection conn;
};


#endif //DATASAVER_ARCHITECTURE_H
