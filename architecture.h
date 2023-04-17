#include <string>
#include <boost/program_options.hpp>
#include <fstream>
#include "structs.h"
#include <pqxx/pqxx>

#ifndef DATASAVER_ARCHITECTURE_H
#define DATASAVER_ARCHITECTURE_H

namespace po = boost::program_options;
using namespace std;

class command_line
{
public:
    command_line(int argc, char* argv[]);
    string run();
    bool f_srch;

private:
    po::options_description desc_;
    string filename_;
    string directory_;
    string search_param;
};

class dataFile {
public:
    explicit dataFile(const string& file_name);
    ~dataFile();
    void readHeaderFromFile();
    void readStringsFromFile();
    vector<string> createQuery();
    void testStructs();

private:
    ifstream datFile;
    string file_name;

    stringStructs firstString;
    stringStructs lastString;
    unsigned int stringInBytes;

    void readOneString(stringStructs *oneString);
    string createQueryIntoFlights();
    string createQueryIntoContext();
    string createQueryIntoSensor();
    string createQueryIntoViewArea();
    string createQueryIntoSeriesOfHolograms();
    string createQueryIntoHologram();
    string createQueryIntoRli();
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
    database(const string& connString);
    ~database();
    void execute(const vector<string>& sqlStatements);
    void search(const string& date);
private:
    pqxx::connection conn;
};


#endif //DATASAVER_ARCHITECTURE_H
