#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

class flights {
    int64_t APMtime;
    int64_t GPStime;
    unsigned int flight_number;
public:
    void readData(FILE *file);
    static void insertData();
};

void flights::readData(FILE *file) {
    fseek(file, 33, SEEK_SET);
    fread(&this->APMtime, 8, 1, file);
    fread(&this->GPStime, 8, 1, file);
    fseek(file, 4, SEEK_CUR);
    fread(&this->flight_number, 4, 1, file);
    this->APMtime /= 1000; //convert to seconds
    this->GPStime /= 1000;
    //convert time to another form
   /* struct tm tm = *localtime(&this->APMtime);
    printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec);
    tm = *localtime(&this->GPStime);
    printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec);*/
}

void flights::insertData() {
    char const *sql;

    try {
        connection C("dbname = datasaver user = keeper password = '' hostaddr = 127.0.0.1 port = 5432");
        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return;
        }

        /* Create SQL statement */
        sql = "INSERT INTO flights (NumFly) " \
              "VALUES (19)";

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
    FILE *pFile;

    pFile = fopen("under_test.K", "r");
    fileCheck(pFile);
    if(pFile == nullptr) perror ("Error opening file");

    flights test{};
    test.readData(pFile);
    test.insertData();
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

