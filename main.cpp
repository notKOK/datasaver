#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

int fileChek(FILE *file);

int main() {
    FILE *pFile;

    pFile = fopen("under_test.K", "r");
    fileChek(pFile);
    if(pFile == nullptr) perror ("Error opening file");
    fclose(pFile);
    cout << "OK" << endl;
    return 0;
}

int fileChek(FILE *file){
    if(file == nullptr) perror ("file == nullptr");
    vector<unsigned int>checkvec { 0x00FF00FF, 0x01FC01FE, 0x01F001F8, 0x56AA55AA };
    size_t checkvecsize = checkvec.size();
    unsigned int buffer;
    for(size_t i = 0; i < checkvecsize; ++i) {
        fread(&buffer, 4, 1, file);
        if(std::count(checkvec.begin(), checkvec.end(), buffer)) {
            continue;
        } else {
            perror("isn't a РЛС и РЛИ файл");
            break;
        }
    }
}

