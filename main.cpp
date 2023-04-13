#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#pragma pack(push, 1)
#include <filesystem>
#include <string>
#include <pqxx/pqxx>
#include <regex>
#include "structs.h"
#include "architecture.h"


#define FILENAME "OS3.k"


using namespace std;
using namespace pqxx;
void insertDataToDB(const char *statem);

unsigned int stringInBytes;

void insert_into_flights(subheading subheader){
    char *sqlStatement = new char[100];
    subheader.APM_time /= 1000; //convert to seconds

    struct tm tm = *localtime(&(subheader.APM_time));
    snprintf(sqlStatement, 100,\
        "INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%d-%d')", \
        0, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
}

void insert_into_context(subheading subheader){
    char *sqlStatement = new char[1000 + 1];
    subheader.APM_time /= 1000; //convert to seconds

    struct tm tm = *localtime(&(subheader.APM_time));
    snprintf(sqlStatement, 1000,\
        "INSERT INTO context (ContextName, ContextBeginDate, ContextEndDate, Latitude,"
             " Longitude, Latitude1, Longitude1, Commentary)"
    "VALUES ('%d-%d-%d', '2022-01-01', '2022-12-31', 40.7128, -74.0060,"
    " 40.7128, -74.0060, NULL);", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
            //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
    delete[] sqlStatement;
}

void insert_into_sensor(locator_operation locatorOperation){
    char *sqlStatement = new char[1000];
    int type = int(locatorOperation.range_number);
    snprintf(sqlStatement, 100,\
        "INSERT INTO sensor (sensortype) VALUES ('РЛС-А%d00')", \
        type); //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
    delete[] sqlStatement;
}

void insert_into_view_area(synchronizer synch){
    char *sqlStatement = new char[1000];
    int SourceDataZone = 0; //always 0
    snprintf(sqlStatement, 1000,\
        "INSERT INTO view_zone (Latitude, Longitude,"
             " Latitude1, Longitude1, StartTime, EndTime,"
             " RangeToTheZone, Side, SourceDataZone)"
    "VALUES (40.7128, -74.0060, 40.7128, -74.0060,"
    " '2022-01-01 00:00:00', '2022-01-01 01:00:00',"
    " %f, %d, %d);",synch.initial_range, synch.side, SourceDataZone);
        //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
    delete[] sqlStatement;
}

void insert_into_series_of_holograms(locator_operation locatorOperation,
                                     synchronizer synch, receiver recev){
    char *sqlStatement = new char[1000];
    int Type_Rgg = 2; //always 2
    int type = int(locatorOperation.range_number);
    filesystem::path filepath(FILENAME);
    string filePath = filesystem::absolute(filepath);
    synch.polarization = (synch.polarization == '0')? 'V' : (synch.polarization == '1')? 'H' :
            (synch.polarization == '2')? '2' : '4';
    recev.polarization = (recev.polarization == '0')? 'V' : 'H';

    snprintf(sqlStatement, 1000,\
        "INSERT INTO series_of_holograms (NumLocator, Type_Rgg,"
             " Type_Work, NumStrAzimuth, NumStrRange, Step_Azimuth,"
             " Step_Range, Series_Rgg, PolarBut,"
             " PolarReception, BandWidth, DiskLabel, Path_Rgg) "
             "VALUES (%d, %d, %d, 12345678, 123456, %f, 2.3,"
             " NULL, '%c', '%c', 456, NULL, '%s');",
             type, Type_Rgg, synch.overview_mode,
             synch.Step_Azimuth, synch.polarization,
             recev.polarization, filePath.c_str());
    //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
    delete[] sqlStatement;
}

void insert_into_hologram(){
    char *sqlStatement = new char[1000];
    snprintf(sqlStatement, 1000,\
        "INSERT INTO hologram (FileName, Num_file)"
             " VALUES ('%s', 12345);", FILENAME);
    //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
    delete[] sqlStatement;
}

void insert_into_rli(locator_operation locatorOperation,
                     synchronizer synch, receiver recev, synthesis synth){
    char *sqlStatement = new char[1000];
    filesystem::path filepath(FILENAME);
    string filePath = filesystem::absolute(filepath);
    synch.polarization = (synch.polarization == '0')? 'V' : (synch.polarization == '1')? 'H' :
                                                            (synch.polarization == '2')? '2' : '4';
    recev.polarization = (recev.polarization == '0')? 'V' : 'H';

    snprintf(sqlStatement, 1000,\
        "INSERT INTO rli (NumLocator, Step_Azimuth, Step_Range,"
             " PolarBut, PolarReception, BandWidth, Form_RLI, FileName, Rli_Type,"
             " AzimuthSize, RangeSize, Commentary)"
             "VALUES (%d, %f, 2.3, '%c', '%c', 456, 0, '%s',"
             " %d, 1234567, 1234567, NULL);",
             locatorOperation.range_number,synth.Step_Azimuth,
             synch.polarization, recev.polarization,
             filePath.c_str(), synch.overview_mode);
    //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
    delete[] sqlStatement;
}

tuple<int64_t, int64_t> readStringFromFile(FILE *file){
    fseek(file, 32, SEEK_CUR); // skip signature string
    navigation navgt{};
    fread(&navgt, 768, 1, file);
    control_receiver receiver{};
    fread(&receiver, 32, 1, file);
    control_transmitter transmitter{};
    fread(&transmitter, 32, 1, file);
    control_synchronizer synchronizer{};
    fread(&synchronizer, 32, 1, file);
    control_generator generator{};
    fread(&generator, 32, 1, file);
    control_JSO jso{};
    fread(&jso, 32, 1, file);
    control_antenna_system antennaSystem{};
    fread(&antennaSystem, 32, 1, file);
    control_ACP acp{};
    fread(&acp, 32, 1, file);

    if(navgt.stringNumber == 0) {
        int64_t ContextBeginDate = navgt.APMtime;
    }
    int64_t ContextEndDate, ContextBeginDate;
    ContextEndDate = navgt.APMtime;
    return make_tuple(ContextBeginDate, ContextEndDate);
}

void readDataFromFile(FILE *file) {
    fseek(file, 32, SEEK_SET); // skip signatures
    subheading subheader{};
    fread(&subheader, 128, 1, file);
    locator_operation locatorOperation{};
    fread(&locatorOperation, 64, 1, file);
    receiver recev{};
    fread(&recev, 64, 1, file);
    transmitter transmt{};
    fread(&transmt, 64, 1, file);
    synchronizer synch{};
    fread(&synch, 64, 1, file);
    generator gen{};
    fread(&gen, 64, 1, file);
    JSO jso{};
    fread(&jso, 64, 1, file);
    antenna_system antennaSystem{};
    fread(&antennaSystem, 64, 1, file);
    ACP acp{};
    fread(&acp, 128, 1, file);

    string filename = FILENAME;
    if (regex_match(filename, regex(".*\\.rl4"))) {
        synthesis synth{};
        fread(&synth, 512, 1, file);
        format_string formatString{};
        fread(&formatString, 64, 1, file);

        insert_into_rli(locatorOperation, synch, recev, synth);
        return;
    }

    format_string formatString{};
    fread(&formatString, 64, 1, file);

//    while ( ! feof (file) ) {
//        auto [ContextBeginDate, ContextEndDate] = readStringFromFile(file);
//        fseek(file, stringInBytes, SEEK_CUR);
//    }

    insert_into_flights(subheader);

    insert_into_sensor(locatorOperation);

    insert_into_context(subheader);

    insert_into_view_area(synch);

    insert_into_series_of_holograms(locatorOperation, synch, recev);

    insert_into_hologram();

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
  //  formatString.dataType =
    stringInBytes = dataSize * formatString.countersInString * 2;
}

void insertDataToDB(const char *statem) {
    try {
        connection C("dbname = datasaver user = keeper password = '' hostaddr = 127.0.0.1 port = 5432");
        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return;
        }

        /* Create a transactional object. */
        work W(C);

        /* Execute SQL query */
        W.exec(statem);
        W.commit();
        cout << "Records created successfully" << endl << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return;
    }
}

void fileCheck(FILE *file){
    if(file == nullptr) perror ("file == nullptr");
    vector<unsigned int>checkvec { 0x00FF00FF, 0x01FC01FE, 0x01F001F8, 0x56AA55AA };
    size_t checkvecsize = checkvec.size();
    unsigned int buffer;
    for(size_t i = 0; i < checkvecsize; ++i) {
        fread(&buffer, 4, 1, file);
        if(std::count(checkvec.begin(), checkvec.end(), buffer)) {
            continue;
        } else {
            perror("isn't a РЛС, РЛИ или строковой файл");
            break;
        }
    }
}

int main() {
    //open file and read signature

    FILE *pFile;
    pFile = fopen(FILENAME, "r");
    fileCheck(pFile);

    //read main data from file
    readDataFromFile(pFile);

    //end
    fclose(pFile);
    cout << "OK " << endl;
    return 0;
}
#pragma pack(pop)

// в мейне создавать экземпляры классов и вызов интерфейсных функций
// чтение мейна - разбор командной строки
//1824 хедер общий
// 784-800 весь заголовок
// 1024??
// 1024 - 64 заголовок строки
// заголовок ргг 800
// короче разберемся