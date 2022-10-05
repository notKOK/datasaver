#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

void readDataFromFile(FILE *file) {
    int64_t APMtime;
    unsigned int flight_number;
    fseek(file, 33, SEEK_SET);
    fread(&APMtime, 8, 1, file);
    fseek(file, 12, SEEK_CUR);
    fread(&flight_number, 4, 1, file);
    APMtime /= 1000; //convert to seconds

    //flights
    struct tm tm = *localtime(&APMtime);
    char result [100];
    snprintf(result, 100,\
        "INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%d-%d')", \
        flight_number, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); //Create SQL statement

    //


}

void insertDataToDB() {
    char const *sql;

    try {
        connection C("dbname = datasaver user = keeper password = '' hostaddr = 127.0.0.1 port = 5432");
        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return;
        }

        sql;

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
    if(pFile == nullptr) perror ("Error opening file");

    //read main data from file
    readDataFromFile(pFile);
    insertDataToDB();

    //end
    fclose(pFile);
    cout << "OK " << endl;
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