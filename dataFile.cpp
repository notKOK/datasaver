#include "architecture.h"
#include "structs.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/format.hpp>



dataFile::dataFile(const std::string& file_name) {
    datFile.open(file_name, std::ios::binary);
    this->file_name = file_name;
    if (datFile.is_open()) {
        unsigned int test;
        datFile.read(reinterpret_cast<char *>(&test), sizeof(test));
        if (test != 0x00FF00FF) perror("isn't a РЛС, РЛИ или строковой файл");
        datFile.read(reinterpret_cast<char *>(&test), sizeof(test));
        if (test != 0x01FC01FE) perror("isn't a РЛС, РЛИ или строковой файл");
        datFile.read(reinterpret_cast<char *>(&test), sizeof(test));
        if (test != 0x01F001F8) perror("isn't a РЛС, РЛИ или строковой файл");
        datFile.read(reinterpret_cast<char *>(&test), sizeof(test));
        if (test != 0x56AA55AA) perror("isn't a РЛС, РЛИ или строковой файл");
    }
    else{
        perror("cant open file");
    }
}

void dataFile::readHeaderFromFile() {
    datFile.seekg(32, std::ifstream::beg); // skip signatures
    datFile.read(reinterpret_cast<char *>(&subheader), sizeof(subheading));
    datFile.read(reinterpret_cast<char *>(&locatorOperation), sizeof(locator_operation));
    datFile.read(reinterpret_cast<char *>(&recev), sizeof(receiver));
    datFile.read(reinterpret_cast<char *>(&transmt), sizeof(transmitter));
    datFile.read(reinterpret_cast<char *>(&synch), sizeof(synchronizer));
    datFile.read(reinterpret_cast<char *>(&gen), sizeof(generator));
    datFile.read(reinterpret_cast<char *>(&jso), sizeof(JSO));
    datFile.read(reinterpret_cast<char *>(&antennaSystem), sizeof(antenna_system));
    datFile.read(reinterpret_cast<char *>(&acp), sizeof(ACP));
    datFile.read(reinterpret_cast<char *>(&formatString), sizeof(format_string));
}

dataFile::~dataFile() {
    datFile.close();
}

void dataFile::readStringsFromFile() {
    unsigned int stringInBytes;
    int dataSize = int(formatString.counterType);
    switch(dataSize) {
        case 0:
            dataSize = sizeof(char);
            break;
        case 1:
            dataSize = sizeof(short);
            break;
        case 2:
            dataSize = sizeof(unsigned char);
            break;
        case 3:
            dataSize = sizeof(unsigned short);
            break;
        case 4:
            dataSize = sizeof(float);
            break;
        case 5:
            dataSize = sizeof(double);
            break;
    }
    //formatString.dataType = if == 0: 2 if == 1: 1
    stringInBytes = dataSize * formatString.countersInString * 2;
    readOneString(&firstString);
    while(true){
        datFile.seekg(stringInBytes, std::ifstream::cur);
        if(datFile.tellg() == -1) break;
        readOneString(&lastString);
        //std::cout << lastString.navgt.stringNumber << std::endl;
    }
}

void dataFile::readOneString(stringStructs *oneString) {
    datFile.seekg(32, std::ifstream::cur); // skip signature string
    datFile.read(reinterpret_cast<char *>(&oneString->navgt), sizeof(oneString->navgt));
    datFile.read(reinterpret_cast<char *>(&oneString->receiver), sizeof(oneString->receiver));
    datFile.read(reinterpret_cast<char *>(&oneString->transmitter), sizeof(oneString->transmitter));
    datFile.read(reinterpret_cast<char *>(&oneString->synchronizer), sizeof(oneString->synchronizer));
    datFile.read(reinterpret_cast<char *>(&oneString->generator), sizeof(oneString->generator));
    datFile.read(reinterpret_cast<char *>(&oneString->jso), sizeof(oneString->jso));
    datFile.read(reinterpret_cast<char *>(&oneString->antennaSystem), sizeof(oneString->antennaSystem));
    datFile.read(reinterpret_cast<char *>(&oneString->acp), sizeof(oneString->acp));
}

std::vector<std::string> dataFile::createQuery(){
    std::vector<std::string> sqlStatements;
    sqlStatements.push_back(createQueryIntoFlights());
    sqlStatements.push_back(createQueryIntoContext());
    sqlStatements.push_back(createQueryIntoSensor());
    sqlStatements.push_back(createQueryIntoViewArea());
    sqlStatements.push_back(createQueryIntoSeriesOfHolograms());
    sqlStatements.push_back(createQueryIntoHologram());
    return sqlStatements;
}


std::string dataFile::createQueryIntoFlights() {
    int64_t time_ = subheader.APM_time / 1000; //convert to seconds

    tm tm = *localtime(&(time_));
    boost::format fmt("INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%02d-%02d')");
    fmt % 0 % (tm.tm_year + 1900) % (tm.tm_mon + 1) % tm.tm_mday; //Set parameters
    std::string sqlStatement = fmt.str();
    std::cout << sqlStatement << std::endl << std::endl;
    return sqlStatement;
}

std::string dataFile::createQueryIntoContext(){
    int64_t time_ = subheader.APM_time / 1000; //convert to seconds
    struct tm tm = *localtime(&(time_));
    time_ = firstString.navgt.APMtime / 1000;
    struct tm tm1 = *localtime(&(time_));
    time_ = lastString.navgt.APMtime / 1000;
    struct tm tm2 = *localtime(&(time_));

    boost::format sqlStatement("INSERT INTO context (ContextName, ContextBeginDate, ContextEndDate, Latitude, Longitude, Latitude1, Longitude1, Commentary) VALUES ('%1%-%2%-%3%', '%4%-%5%-%6%', '%7%-%8%-%9%', %10%, %11%, %12%, %13%, NULL);");
    sqlStatement % (tm.tm_year + 1900) % (tm.tm_mon + 1) % (tm.tm_mday) % (tm1.tm_year) % (tm1.tm_mon) % (tm1.tm_mday)
                 % (tm2.tm_year) % (tm2.tm_mon) % (tm2.tm_mday) % (firstString.navgt.latitude) % (firstString.navgt.longitude)
                 % (lastString.navgt.latitude) % (lastString.navgt.longitude);
    std::cout << sqlStatement.str() << std::endl << std::endl;
    return sqlStatement.str();
}

std::string dataFile::createQueryIntoSensor() {
    boost::format sqlStatement("INSERT INTO sensor (sensortype) VALUES ('РЛС-А%d00');");
    int type = static_cast<int>(locatorOperation.range_number);
    sqlStatement % type;
    std::cout << sqlStatement.str() << std::endl << std::endl;
    return sqlStatement.str();
}

std::string dataFile::createQueryIntoViewArea() {
    boost::format sqlStatement("INSERT INTO view_zone (Latitude, Longitude, Latitude1, Longitude1, StartTime, EndTime, RangeToTheZone, Side, SourceDataZone) \
    VALUES (%f, %f, %f, %f, '%s', '%s', %f, %d, %d);");
    int SourceDataZone = 0;
    sqlStatement % 40.7128 % -74.0060 % 40.7128 % -74.0060 % "2022-01-01 00:00:00" % "2022-01-01 01:00:00" % synch.initial_range % int(synch.side) % SourceDataZone;
    std::cout << sqlStatement.str() << std::endl << std::endl;
    return sqlStatement.str();
}


std::string dataFile::createQueryIntoSeriesOfHolograms() {
    int Type_Rgg = 2; //always 2
    int type = static_cast<int>(locatorOperation.range_number);
    std::filesystem::path filepath(file_name);
    std::string filePath = std::filesystem::absolute(filepath);
    synch.polarization = (synch.polarization == '0') ? 'V' : (synch.polarization == '1') ? 'H' :
                                                             (synch.polarization == '2') ? '2' : '4';
    recev.polarization = (recev.polarization == '0') ? 'V' : 'H';

    boost::format fmt("INSERT INTO series_of_holograms (NumLocator, Type_Rgg, Type_Work, NumStrAzimuth,"
                      " NumStrRange, Step_Azimuth, Step_Range, Series_Rgg, PolarBut, PolarReception,"
                      " BandWidth, DiskLabel, Path_Rgg) "
                      "VALUES (%1%, %2%, %3%, 12345678, 123456, %4%, 2.3, NULL, '%5%', '%6%', 456,"
                      " NULL, '%7%');");
    fmt % type % Type_Rgg % int(synch.overview_mode) % synch.Step_Azimuth % synch.polarization % recev.polarization
    % filePath;

    std::string sqlStatement = fmt.str();
    std::cout << sqlStatement << std::endl << std::endl;
    return sqlStatement;
}

std::string dataFile::createQueryIntoHologram() {
    boost::format fmt("INSERT INTO hologram (FileName, Num_file) VALUES ('%s', 12345);");
    fmt % file_name;
    std::string sqlStatement = fmt.str();
    std::cout << sqlStatement << std::endl << std::endl;
    return sqlStatement;
}

std::string dataFile::createQueryIntoRli() {
    std::string filePath = std::filesystem::absolute(std::filesystem::path(file_name)).string();
    synch.polarization = (synch.polarization == '0') ? 'V' : (synch.polarization == '1') ? 'H' :
                                                             (synch.polarization == '2') ? '2' : '4';
    recev.polarization = (recev.polarization == '0') ? 'V' : 'H';

    boost::format fmt("INSERT INTO rli (NumLocator, Step_Azimuth, Step_Range,"
                      " PolarBut, PolarReception, BandWidth, Form_RLI, FileName, Rli_Type,"
                      " AzimuthSize, RangeSize, Commentary)"
                      "VALUES (%1%, %2%, 2.3, '%3%', '%4%', 456, 0, '%5%', %6%, 1234567, 1234567, NULL);");
    fmt % locatorOperation.range_number % synch.Step_Azimuth % synch.polarization
    % recev.polarization % filePath % synch.overview_mode;

    std::string sqlStatement = fmt.str();
    return sqlStatement;
}

void dataFile::testStructs() {
    cout << sizeof(firstString.navgt) << endl;
    cout << sizeof(firstString.receiver) << endl;
    cout << sizeof(firstString.transmitter) << endl;
    cout << sizeof(firstString.synchronizer) << endl;
    cout << sizeof(firstString.generator) << endl;
    cout << sizeof(firstString.jso) << endl;
    cout << sizeof(firstString.antennaSystem) << endl;
    cout << sizeof(firstString.acp) << endl;
}
