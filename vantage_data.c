#include "vantage_data.h"
#include <stdio.h>
#include <math.h>

// ****** Pressure
double getBaro(uint16_t baro){
    double hpa = (double)baro/1000 ;
    hpa = ceil(hpa*33.863886666667*10)/10;
    return hpa;
}

// ****** Temperature
double getTemp(uint16_t temp){
    double tmp = (double)temp/10 ;
    tmp = (tmp-32) * 0.5555555555555556;
    tmp = ceil(tmp*10)/10;
    return tmp;
}

// ****** Temperature C->F
double cTof(double temp){
    double tmp = (temp*1.8)+32;
    tmp = ceil(tmp*10)/10;
    return tmp;
}


// ****** Wind speed
double getSpeed(uint16_t sped){    //double tmp = (double)sped;
    return ceil((double)sped * 1.609344*10)/10;
}

// ****** Rain
double getRain(uint16_t rain){
    return ceil((double)rain * 0.2 *10)/10;
}

// ****** Napięcie baterii
double getVolt(uint16_t volt){
    return (((double)volt * 300)/512)/100;;
}

// ****** Hours
 void getGodz(uint16_t godz, char *out){
    int h=godz/100;
    int m=godz-h*100;
    sprintf(out, "%d:%d", h, m);
}

// ****** Swap two bytes position
 uint16_t byteSwap(uint16_t w){
    return ((w & 0x00FF)<<8)+(w>>8);
}

// ****** Dev point
double getDew(double temp, int wilg){
    return ceil( (pow(((double)wilg/100), 0.125)*(112+(0.9*temp))+(0.1*temp)-112) *10 )/10;
}

// ****** Seek "LOO"
int findLOO(char *bufferRx, int length){
    int r=-1;
    for (int i=0; i<length-2; i++){
        if (bufferRx[i+0] == 'L' &&
            bufferRx[i+1] == 'O' &&
            bufferRx[i+1] == 'O')
        {
            r=i;
            break;
        }

    }

    return r;
}
