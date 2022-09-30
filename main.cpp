#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

class flights {
    int64_t APMtime;
    int64_t GPStime;
    unsigned int flight_number;
public:
    void readdata(FILE *file);
};

void flights::readdata(FILE *file) {
    fseek(file, 33, SEEK_SET);
    fread(&this->APMtime, 8, 1, file);
    fread(&this->GPStime, 8, 1, file);
    fseek(file, 4, SEEK_CUR);
    fread(&this->flight_number, 4, 1, file);
    this->APMtime /= 1000;
    this->GPStime /= 1000;

    //convert time to another form
   /* struct tm tm = *localtime(&this->APMtime);
    printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec);
    tm = *localtime(&this->GPStime);
    printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec);*/
}


int fileCheck(FILE *file);

int main() {
    FILE *pFile;

    pFile = fopen("under_test.K", "r");
    fileCheck(pFile);
    if(pFile == nullptr) perror ("Error opening file");

    flights test{};
    test.readdata(pFile);
    fclose(pFile);
    cout << "OK " << endl;
    return 0;
}

int fileCheck(FILE *file){
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

