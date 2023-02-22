#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <pqxx/pqxx>
#include "main.h"

using namespace std;
using namespace pqxx;

void readDataFromFile(FILE *file, char *result) {
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

    subheader.APM_time /= 1000; //convert to seconds

    //flights
    struct tm tm = *localtime(&(subheader.APM_time));
    snprintf(result, 100,\
        "INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%d-%d')", \
        0, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); //Create SQL statement
}

void insertDataToDB(const char *statem) {
    char const *sql = statem;

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
        W.exec( sql );
        W.commit();
        cout << "Records created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return;
    }
}


void fileCheck(FILE *file);

int main() {
    //open file and read signature
    FILE *pFile;
    pFile = fopen("under_test.K", "r");
    fileCheck(pFile);

    //read main data from file
    char *sqlStatement = new char[100];
    readDataFromFile(pFile, sqlStatement);
    insertDataToDB(sqlStatement);

    //end
    fclose(pFile);
    cout << "OK " << endl;

    pFile = fopen("under_test.K", "r");
    heading test{};
    fread(&test, 32, 1, pFile);
    if (test.sig1 == 0x00FF00FF) cout << sizeof(int64_t) << endl;
    return 0;
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
