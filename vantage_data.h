#include <stdint.h>

//Data structure of command "LOOP 1"
struct __attribute__((__packed__)) loop1
{
    char        _loo[3];     //0
    int8_t      _Bar_Trend;  //3
    uint8_t     _PacketType; //4
    uint16_t    _NextRecord; //5
    uint16_t    _Barometr;   //7
    int16_t     _Inside_Temperature;     //9
    uint8_t     _Inside_Humidity;        //11
    int16_t     _Outside_Temperature;    //12
    uint8_t     _Wind_Speed;             //14
    uint8_t     _10Min_Avg_Wind_Speed;   //15
    uint16_t    _Wind_Direction;         //16
    uint8_t     _Extra_Temperatures[7];  //18
    uint32_t    _Soil_Temperatures;  //25
    uint32_t    _Leaf_Temperatures;  //29
    uint8_t     _Outside_Humidity;   //33
    uint8_t     _Extra_Humidties[7]; //34
    uint16_t    _Rain_Rate;          //41
    uint8_t     _UV;                 //43
    uint16_t    _Solar_Radiation;    //44
    uint16_t    _Storm_Rain;         //46
    uint16_t    _Start_Date_of_currentStorm;//48
    uint16_t    _Day_Rain;   //50
    uint16_t    _Month_Rain; //52
    uint16_t    _Year_Rain;  //54
    uint16_t    _Day_ET;     //56
    uint16_t    _Month_ET;   //58
    uint16_t    _Year_ET;    //60
    uint32_t    _Soil_Moistures; //62
    uint32_t    _Leaf_Wetnesses; //66
    uint8_t     _Inside_Alarms;  //70
    uint8_t     _Rain_Alarms;    //71
    uint16_t    _Outside_Alarms; //72
    uint8_t     _Extra_TempHum_Alarms[8];    //74
    uint32_t    _Soil_and_Leaf_Alarms;       //82
    uint8_t     _Transmitter_Battery_Status; //86
    uint16_t    _Console_Battery_Voltage;    //87
    uint8_t     _Forecast_Icons;             //89
    uint8_t     _Forecast_Rule_number;       //90
    uint16_t    _Time_of_Sunrise;            //91
    uint16_t    _Time_of_Sunset;             //93
    char        _lf;                         //95
    char        _cr;                         //96
    uint16_t    _crc;                        //97
};


// ****** Pressure
double getBaro(uint16_t baro);

// ****** Temperature
double getTemp(uint16_t temp);

// ****** Temperature C->F
double cTof(double temp);

// ****** Wind speed
double getSpeed(uint16_t sped);

// ****** Rain
double getRain(uint16_t rain);

// ****** Battery voltage (super capacitor)
double getVolt(uint16_t volt);

// ****** Hours
void getGodz(uint16_t godz, char *out);

// ****** Swap two bytes position
uint16_t byteSwap(uint16_t w);

// ****** Dev point
double getDew(double temp, int wilg);

// ****** Seek "LOO"
int findLOO(char *bufferRx, int length);
