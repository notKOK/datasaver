#include "architecture.h"
#include "structs.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/format.hpp>


DataFile::DataFile(const string &file_name) {
    datFile.open(file_name, ios::binary);
    this->file_name = file_name;
    if (datFile.is_open()) {
        datFile.read(headingBufer.bytes, sizeof(headingBufer)); //sizeof(headingBufer)
        if (headingBufer._heading.sig1 != 0x00FF00FF) perror("isn't a РЛС, РЛИ или строковой файл");
        if (headingBufer._heading.sig2 != 0x01FC01FE) perror("isn't a РЛС, РЛИ или строковой файл");
        if (headingBufer._heading.sig3 != 0x01F001F8) perror("isn't a РЛС, РЛИ или строковой файл");
        if (headingBufer._heading.sig4 != 0x56AA55AA) perror("isn't a РЛС, РЛИ или строковой файл");
    } else {
        perror("cant open file");
    }
}

void DataFile::readHeaderFromFile() {
    datFile.seekg(32, ifstream::beg); // skip signatures
    datFile.read(subheader.bytes, sizeof(subheading));
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

DataFile::~DataFile() {
    datFile.close();
}

void DataFile::readStringsFromFile() {
    int dataSize = int(formatString.counterType);
    switch (dataSize) {
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
    short int datatype;
    if (int(formatString.dataType) == 0) datatype = 2;
    else {
        datatype = 1;
    }
    stringInBytes = dataSize * formatString.countersInString * datatype;
    readOneString(&firstString);
    while (true) {
        datFile.seekg(stringInBytes, ifstream::cur);
        if (datFile.tellg() == -1) break;
        readOneString(&lastString);
        //cout << lastString.navgt.stringNumber << endl;
    }
}

void DataFile::readOneString(stringStructs *oneString) {
    datFile.seekg(32, ifstream::cur); // skip signature string
    datFile.read(reinterpret_cast<char *>(&oneString->navgt), sizeof(oneString->navgt));
    datFile.read(reinterpret_cast<char *>(&oneString->receiver), sizeof(oneString->receiver));
    datFile.read(reinterpret_cast<char *>(&oneString->transmitter), sizeof(oneString->transmitter));
    datFile.read(reinterpret_cast<char *>(&oneString->synchronizer), sizeof(oneString->synchronizer));
    datFile.read(reinterpret_cast<char *>(&oneString->generator), sizeof(oneString->generator));
    datFile.read(reinterpret_cast<char *>(&oneString->jso), sizeof(oneString->jso));
    datFile.read(reinterpret_cast<char *>(&oneString->antennaSystem), sizeof(oneString->antennaSystem));
    datFile.read(reinterpret_cast<char *>(&oneString->acp), sizeof(oneString->acp));
}

vector<string> DataFile::createQuery() {
    vector<string> sqlStatements;
    sqlStatements.push_back(createQueryIntoFlights());
    sqlStatements.push_back(createQueryIntoContext());
    sqlStatements.push_back(createQueryIntoSensor());
    sqlStatements.push_back(createQueryIntoViewArea());
    sqlStatements.push_back(createQueryIntoSeriesOfHolograms());
    sqlStatements.push_back(createQueryIntoHologram());
    return sqlStatements;
}

string DataFile::createQueryIntoFlights() {
    int64_t time_ = subheader._subheading.APM_time / 1000; //convert to seconds

    tm tm = *localtime(&(time_));
    boost::format fmt("INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%02d-%02d')");
    fmt % 0 % (tm.tm_year + 1900) % (tm.tm_mon + 1) % tm.tm_mday; //Set parameters
    string sqlStatement = fmt.str();
    cout << sqlStatement << endl << endl;
    return sqlStatement;
}

string DataFile::createQueryIntoContext() {
    int64_t time_ = subheader._subheading.APM_time / 1000; //convert to seconds
    struct tm tm = *localtime(&(time_));
    time_ = firstString.navgt.APMtime / 1000;
    struct tm tm1 = *localtime(&(time_));
    time_ = lastString.navgt.APMtime / 1000;
    struct tm tm2 = *localtime(&(time_));

    boost::format sqlStatement(
            "INSERT INTO context (ContextName, ContextBeginDate, ContextEndDate, Latitude, Longitude, Latitude1, Longitude1, Commentary) VALUES ('%1%-%2%-%3%', '%4%-%5%-%6% %7%:%8%:%9%', '%10%-%11%-%12% %13%:%14%:%15%', %16%, %17%, %18%, %19%, NULL);");
    sqlStatement % (tm.tm_year + 1900) % (tm.tm_mon + 1) % (tm.tm_mday)
    % (tm1.tm_year + 1900) % (tm1.tm_mon + 1) % (tm1.tm_mday) % (tm1.tm_hour) % (tm1.tm_min) % (tm1.tm_sec)
    % (tm2.tm_year + 1900) % (tm2.tm_mon + 1) % (tm2.tm_mday) % (tm2.tm_hour) % (tm2.tm_min) % (tm2.tm_sec)
    % (firstString.navgt.latitude) % (firstString.navgt.longitude)
    % (lastString.navgt.latitude) % (lastString.navgt.longitude);
    cout << sqlStatement.str() << endl << endl;
    return sqlStatement.str();
}

string DataFile::createQueryIntoSensor() {
    boost::format sqlStatement("INSERT INTO sensor (sensortype) VALUES ('РЛС-А%d00');");
    int type = static_cast<int>(locatorOperation.range_number);
    sqlStatement % type;
    cout << sqlStatement.str() << endl << endl;
    return sqlStatement.str();
}

string DataFile::createQueryIntoViewArea() {
    int64_t time_ = firstString.navgt.APMtime / 1000;
    struct tm tm1 = *localtime(&(time_));
    time_ = lastString.navgt.APMtime / 1000;
    struct tm tm2 = *localtime(&(time_));

    boost::format sqlStatement("INSERT INTO view_zone (Latitude, Longitude, Latitude1, Longitude1, StartTime, EndTime, RangeToTheZone, Side, SourceDataZone) \
    VALUES (%f, %f, %f, %f, '%04d-%02d-%02d %02d:%02d:%02d', '%04d-%02d-%02d %02d:%02d:%02d', %f, %d, %d);");
    int SourceDataZone = 0;
    sqlStatement % (firstString.navgt.latitude) % (firstString.navgt.longitude)
    % (lastString.navgt.latitude) % (lastString.navgt.longitude)
    % (tm1.tm_year + 1900) % (tm1.tm_mon + 1) % (tm1.tm_mday) % (tm1.tm_hour) % (tm1.tm_min) % (tm1.tm_sec)
    % (tm2.tm_year + 1900) % (tm2.tm_mon + 1) % (tm2.tm_mday) % (tm2.tm_hour) % (tm2.tm_min) % (tm2.tm_sec)
    % synch.initial_range % int(synch.side) % SourceDataZone;
    cout << sqlStatement.str() << endl << endl;
    return sqlStatement.str();
}

string DataFile::createQueryIntoSeriesOfHolograms() {
    int Type_Rgg = 2; //always 2
    int type = static_cast<int>(locatorOperation.range_number);
    filesystem::path filepath(file_name);
    string filePath = filesystem::absolute(filepath);
    synch.polarization = (synch.polarization == '0') ? 'V' : (synch.polarization == '1') ? 'H' :
                                                             (synch.polarization == '2') ? '2' : '4';
    recev.polarization = (recev.polarization == '0') ? 'V' : 'H';

    uintmax_t NumStrAzimuth;
    uintmax_t fileSize = filesystem::file_size(filePath);
    fileSize = fileSize - 800;
    NumStrAzimuth = fileSize / (stringInBytes + 1024);

    cout << "File size: " << fileSize << " bytes\n";
    boost::format fmt("INSERT INTO series_of_holograms (NumLocator, Type_Rgg, Type_Work, NumStrAzimuth,"
                      " NumStrRange, Step_Azimuth, Step_Range, Series_Rgg, PolarBut, PolarReception,"
                      " BandWidth, DiskLabel, Path_Rgg) "
                      "VALUES (%1%, %2%, %3%, %8%, NULL, %4%, NULL, NULL, '%5%', '%6%', NULL,"
                      " NULL, '%7%');");
    fmt % type % Type_Rgg % int(synch.overview_mode) % synch.Step_Azimuth % synch.polarization % recev.polarization
    % filePath % NumStrAzimuth;

    string sqlStatement = fmt.str();
    cout << sqlStatement << endl << endl;
    return sqlStatement;
}

string DataFile::createQueryIntoHologram() {
    boost::format fmt("INSERT INTO hologram (FileName, Num_file) VALUES ('%s', 1);");
    fmt % file_name; //Num_file
    string sqlStatement = fmt.str();
    cout << sqlStatement << endl << endl;
    return sqlStatement;
}

string DataFile::createQueryIntoRli() {
    string filePath = filesystem::absolute(filesystem::path(file_name)).string();
    synch.polarization = (synch.polarization == '0') ? 'V' : (synch.polarization == '1') ? 'H' :
                                                             (synch.polarization == '2') ? '2' : '4';
    recev.polarization = (recev.polarization == '0') ? 'V' : 'H';

    boost::format fmt("INSERT INTO rli (NumLocator, Step_Azimuth, Step_Range,"
                      " PolarBut, PolarReception, BandWidth, Form_RLI, FileName, Rli_Type,"
                      " AzimuthSize, RangeSize, Commentary)"
                      "VALUES (%1%, %2%, NULL, '%3%', '%4%', 456, 0, '%5%', %6%, 1234567, 1234567, NULL);");
    fmt % locatorOperation.range_number % synch.Step_Azimuth % synch.polarization
    % recev.polarization % filePath % synch.overview_mode;

    string sqlStatement = fmt.str();
    return sqlStatement;
}

void DataFile::testStructs() {
    cout << sizeof(firstString.navgt) << endl;
    cout << sizeof(firstString.receiver) << endl;
    cout << sizeof(firstString.transmitter) << endl;
    cout << sizeof(firstString.synchronizer) << endl;
    cout << sizeof(firstString.generator) << endl;
    cout << sizeof(firstString.jso) << endl;
    cout << sizeof(firstString.antennaSystem) << endl;
    cout << sizeof(firstString.acp) << endl;
}
