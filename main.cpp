#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#pragma pack(push, 1)
#include <pqxx/pqxx>
#include "main.h"

using namespace std;
using namespace pqxx;
void insertDataToDB(const char *statem);

unsigned int stringInBytes;

void insert_into_flights(subheading &subheader){
    char *sqlStatement = new char[100];
    subheader.APM_time /= 1000; //convert to seconds

    struct tm tm = *localtime(&(subheader.APM_time));
    snprintf(sqlStatement, 100,\
        "INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%d-%d')", \
        0, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
}

void insert_into_context(){

}

void insert_into_sensor(locator_operation &locatorOperation){
    char *sqlStatement = new char[1000];
    int type = int(locatorOperation.range_number);
    snprintf(sqlStatement, 100,\
        "INSERT INTO sensor (sensortype) VALUES ('РЛС-А%d00')", \
        type); //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
}

void insert_into_view_area(){

}

void insert_into_series_of_holograms(){

}

void insert_into_hologram(){

}

void insert_into_rli(){

}

void readStringFromFile(FILE *file){
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

    static int testDELETEME = 0;
    testDELETEME++;
    navgt.APMtime /= 1000;
    struct tm tm = *localtime(&(navgt.APMtime));
    //cout << testDELETEME <<  endl << tm.tm_year << '-' << tm.tm_mon << '-'<< tm.tm_mday << endl;
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
    format_string formatString{};
    fread(&formatString, 64, 1, file);

    insert_into_sensor(locatorOperation);

    insert_into_flights(subheader);

    /*int dataSize = int(formatString.counterType);
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
    stringInBytes = dataSize * formatString.countersInString;*/
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
        cout << "Records created successfully" << endl;
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
    pFile = fopen("under_test.K", "r");
    fileCheck(pFile);

    //read main data from file
    readDataFromFile(pFile);

    /*while ( ! feof (pFile) ) {
        readStringFromFile(pFile);
        fseek(pFile, stringInBytes, SEEK_CUR);
    }*/

    //end
    fclose(pFile);
    cout << "OK " << endl;
    return 0;
}
#pragma pack(pop)