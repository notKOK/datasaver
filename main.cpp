#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

struct heading {
    unsigned int sig1, sig2, sig3, sig4; //сигнатуры
    unsigned char code; // код заголовка
    unsigned char ver_head; // версия заголовка
    unsigned char reserve[14]; // 32 bytes
};

struct subheading {
    unsigned char ver_heading; // Версия заголовка
    int64_t APM_time;  //Время АРМ
    int64_t GPS_time;   //Время GPS
    unsigned int mission_number; //номер миссии
    unsigned int flight_number; // Номер полета
    unsigned int period_number; // номер периода
    unsigned int file_number; // номер файла
    char country_flight[16]; // страна полета
    char flight_territory[16]; // Территория полета
    unsigned char reserve[63]; // 128 bytes
};


void readDataFromFile(FILE *file, char *result) {
    int64_t APMtime;
    unsigned int flight_number;
    fseek(file, 33, SEEK_SET);
    fread(&APMtime, 8, 1, file);
    fseek(file, 12, SEEK_CUR);
    fread(&flight_number, 4, 1, file);
    APMtime /= 1000; //convert to seconds

    //flights
    struct tm tm = *localtime(&APMtime);
    snprintf(result, 100,\
        "INSERT INTO flights (NumFly, DateTime) VALUES (%d, '%d-%d-%d')", \
        flight_number, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); //Create SQL statement
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
    if(pFile == nullptr) perror ("Error opening file");

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