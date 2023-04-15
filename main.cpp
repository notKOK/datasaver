#include <iostream>
#include <string>
#include <pqxx/pqxx>
#pragma pack(push, 1)
#include "architecture.h"


using namespace std;
using namespace pqxx;

/*
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
    filesystem::path filepath("os.k");
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
             " VALUES ('%s', 12345);", "FILENAME");
    //Create SQL statement
    printf("%s \n", sqlStatement);
    insertDataToDB(sqlStatement);
    delete[] sqlStatement;
}

void insert_into_rli(locator_operation locatorOperation,
                     synchronizer synch, receiver recev, synthesis synth){
    char *sqlStatement = new char[1000];
    filesystem::path filepath("os.k");
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

*/

int main(int argc, char* argv[]) {
    command_line cmd(argc, argv);
    std::string file_name = cmd.run();

    dataFile f(file_name);
    f.readHeaderFromFile();
    f.readStringsFromFile();

    database base("dbname = datasaver user = keeper password = '' hostaddr = 127.0.0.1 port = 5432");
    base.execute(f.createQuery());
    cout << "OK " << endl;
    return 0;
}
#pragma pack(pop)