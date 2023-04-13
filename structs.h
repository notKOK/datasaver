//
// Created by danila on 21.02.23.
//

#ifndef DATASAVER_STRUCTS_H
#define DATASAVER_STRUCTS_H

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

struct locator_operation { //Структура подзаголовка режима работы локатора
    unsigned char version_header;
    unsigned char range_number;
    unsigned char reserve[62]; //reserve 64
};

struct receiver { //Структура подзаголовка параметров приемника
    unsigned char header_version;
    unsigned char polarization;
    unsigned char reserve[62]; //reserve 64
};

struct transmitter { //Структура подзаголовка параметров передатчика
    unsigned char reserve[64];
};

struct synchronizer { //Структура подзаголовка параметров синхронизатора
    unsigned char header_version;
    unsigned char overview_mode;
    unsigned char side;
    unsigned char polarization;
    float initial_range;
    unsigned char reserve1[5];
    float Step_Azimuth;
    unsigned char reserve[47]; //reserve 64
};

struct generator { //Структура подзаголовка параметров генератора
    unsigned char reserve[64];
};

struct JSO { //Структура подзаголовка параметров ЖСО
    unsigned char reserve [64];
};

struct antenna_system { //Структура подзаголовка параметров антенной системы
    unsigned char reserve[64];
};

struct ACP { //Структура подзаголовка параметров АЦП
    unsigned char reserve[128];
};

struct format_string { //Структура подзаголовка формата строки
    unsigned char dataType;
    unsigned char counterType;
    unsigned char orderData;
    unsigned int countersInString;
    unsigned char reserve[57]; //reserve 64
};

class stringStructs {
    // структуры строки
    struct navigation { //Структура подзаголовка параметров навигации
        unsigned char headerVersion;
        int64_t APMtime;
        int64_t GPStime;
        unsigned int stringNumber;
        bool DataSNP;
        double height;
        double angle;
        double latitude;
        unsigned char reserve[722]; //reserve 768
    };

    struct control_receiver {  //Структура контрольных параметров приемника
        unsigned char version_header;
        unsigned char IP;
        unsigned char reserve[30]; //reserve 32
    };

    struct control_transmitter { //Структура контрольных параметров передатчика
        unsigned char reserve[32];
    };

    struct control_synchronizer { //Структура контрольных параметров синхронизатора
        unsigned char reserve[32];
    };

    struct control_generator { //Структура контрольных параметров генератора
        unsigned char reserve[32];
    };

    struct control_JSO { //Структура контрольных параметров СЖО
        unsigned char reserve[32];
    };

    struct control_antenna_system { //Структура контрольных параметров антенной системы
        unsigned char reserve[32];
    };

    struct control_ACP { //Структура контрольных параметров АЦП
        unsigned char reserve[32];
    };

    struct synthesis {
        unsigned char reserve1 [18];
        float Step_Range;
        float Step_Azimuth;
        unsigned char reserve [486];
    };
};
#endif //DATASAVER_STRUCTS_H
