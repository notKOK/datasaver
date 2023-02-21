//
// Created by danila on 21.02.23.
//

#ifndef DATASAVER_MAIN_H
#define DATASAVER_MAIN_H

#pragma pack(push, 1)

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

#pragma pack(pop)
#endif //DATASAVER_MAIN_H
