#include "architecture.h"
#include "structs.h"
#include <iostream>
#include <string>
#include <fstream>


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

void dataFile::readDataFromFile() {
    datFile.seekg(32); // skip signatures
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
    std::cout << datFile.tellg() << std::endl;
}

subheading dataFile::davot() {
    return subheader;
}

dataFile::~dataFile() {
    datFile.close();
}
