#include "architecture.h"
#include "structs.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/format.hpp>



dataFile::dataFile(const std::string& file_name) {
    datFile.open(file_name, std::ios::binary);
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
    return sqlStatements;
}


std::string dataFile::createQueryIntoFlights() {
    int64_t time_ = subheader.APM_time / 1000; //convert to seconds

    tm tm = *localtime(&(time_));
    boost::format fmt("INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%02d-%02d')");
    fmt % 0 % (tm.tm_year + 1900) % (tm.tm_mon + 1) % tm.tm_mday; //Set parameters
    std::string sqlStatement = fmt.str();
    std::cout << sqlStatement << std::endl;
    return sqlStatement;
}


