#include <iostream>
#include <string>
#pragma pack(push, 1)
#include "architecture.h"

using namespace std;

int main(int argc, char* argv[]) {
    command_line cmd(argc, argv);
    string file_name = cmd.run();

    if(cmd.f_srch) {
        dataFile f(file_name);
        f.readHeaderFromFile();
        f.readStringsFromFile();
        //f.testStructs();
        database base("dbname = datasaver user = keeper password = '' hostaddr = 127.0.0.1 port = 5432");
        base.execute(f.createQuery());
    }
    else {
        database base("dbname = datasaver user = keeper password = '' hostaddr = 127.0.0.1 port = 5432");
        base.search(file_name);
    }

    cout << "OK " << endl;
    return 0;
}
#pragma pack(pop)