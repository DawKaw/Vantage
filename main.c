#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "vantage_data.h"
#include "crc16-ccitt.h"

#define BUF_SIZE 128
#define MAX_PROBE 8

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{

    char *s_addr = "";
    uint16_t i_port = 22222;
    int  opt;
    int imperial = 0;
    while ((opt = getopt(argc, argv, "ia:p:")) != -1) {

        switch (opt) {
            case 'i':
                imperial = 1;
                break;
            case 'a':
                s_addr = optarg;
                break;
           case 'p':
                i_port = atoi(optarg);
                break;

            default: /* '?' */
                printf("Usage: %s -a address [ -p port, -i]\n", argv[0]);
                printf("    [-a] device host name or ip address\n");
                printf("    [-p] device ip port (default 22222)\n");
                printf("    [-i] imperial units\n");
                printf("Example:\n  %s -a 192.168.0.127\n", argv[0]);
                printf("  %s -a 192.168.0.127 -p 22222 -i\n", argv[0]);
                exit(EXIT_FAILURE);
            break;
        }
    }


    if (strlen(s_addr)<5){
         fprintf(stderr, "Device address parameter is missing.\n");
         return EXIT_FAILURE;
    }

    if (i_port<1){
        fprintf(stderr, "Device ip port parameter is incorrect\n");
        return EXIT_FAILURE;

    }


    //Prepare socket
    int soket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (soket1 < 0) { error("socket()"); }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(s_addr);
    server_addr.sin_port = htons(i_port);

    //soket timeout
    struct timeval tv;
    tv.tv_sec = 5;        // 5 Secs Timeout
    tv.tv_usec = 0;       // Not init'ing this can cause strange errors
    setsockopt(soket1, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
    setsockopt(soket1, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv,sizeof(struct timeval));

    if (connect(soket1, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("Connectionn with host is unavailable.");


    usleep(100*1000);   //Wait 100ms




    //Prepare cleared bufor
    char bufferRx[BUF_SIZE*2];
    bzero(bufferRx, BUF_SIZE*2);






    //Wakeup device (3x try)
    int n;
    int l = 0;
    char *cmdTx = "\n";
    do {

        //Send wakeip command "\n"
        write(soket1, cmdTx, strlen(cmdTx));

        usleep(1200*1000);   //wait 1.2 sek

        //Read ack
        n = read(soket1, bufferRx, 2);

        //Check if reply then ... "\n\r"
        if (n==2 && bufferRx[0]=='\n' && bufferRx[1]=='\r')
           l=MAX_PROBE; //Is OK, than cause exit from loop.
        else {
           l++;
        }

    } while (l < MAX_PROBE);

    if (l==MAX_PROBE-1){
        fprintf(stderr, "Device does'nt responding on command wake up.\n");
        close(soket1);
        return 1;
    }









    //Send command LOOP 1 (3x try)
    cmdTx = "LOOP 1\r\n";
    l = 0;
    do {

        //Send wakeup command "\n"
        write(soket1, cmdTx, strlen(cmdTx));

        //Read ack
        n = read(soket1, bufferRx, 1);

        //Check if reply is "0x06"
        if (n==1 && bufferRx[0]==0x06)
           l=MAX_PROBE; //Is OK, then cause exit from loop.
        else {
           l++;
           usleep(1200*1000); //Wait 1.2 sek and try again
           //printf("Repeat.");
        }


    } while (l < MAX_PROBE-1);

    if (l==MAX_PROBE-1){
       fprintf(stderr, "Device does'nt accept LOOP 1 command.\n");
       close(soket1);
       return 1;
    }








    //Loop until receive correct data length
    n = 0;
    while (n < sizeof(struct loop1))
        n += read(soket1, &bufferRx[n], BUF_SIZE-1);


    close(soket1);

    //printf("Received: %d\n", n);

    //Save rav dara to the file
    //FILE *f = fopen("raw.bin","wb+");
    //fwrite(bufferRx, n, 1, f);
    //fclose(f);

    if (n>=sizeof(struct loop1)) {
        int poz = findLOO(bufferRx, BUF_SIZE-1);    //Catch header position of frame "LOO"
        //printf("frame begin at : %d\n", poz);

        if (poz>=0){

            struct loop1 *buf;
            buf = (struct loop1*) &bufferRx[poz];   //Skip begining byte


            //Check CRC
            unsigned short sendedCRC = byteSwap(buf->_crc);
            unsigned short calculatedCRC = crc16_ccitt(bufferRx, sizeof(struct loop1)-2, 0);
            if (sendedCRC != calculatedCRC){
                fprintf(stderr, "CRC of received data not pass!\n");
                return 1;
            }

            if (imperial==1) {

                //Parse data in imperialnych units
                printf("outside_temperature %.1f\n",(double)buf->_Outside_Temperature/10);
                printf("outside_humidity %d\n",     buf->_Outside_Humidity);
                printf("dew_point_outside %.1f\n", cTof( getDew(getTemp(buf->_Outside_Temperature), buf->_Outside_Humidity)) );

                printf("barometr %.1f\n",           (double)buf->_Barometr/1000);
                printf("bar_trend %d\n", buf->_Bar_Trend);
                printf("10min_avg_wind_speed %d\n", buf->_10Min_Avg_Wind_Speed);
                printf("wind_speed %d\n",           buf->_Wind_Speed);
                printf("wind_direction %d\n",       buf->_Wind_Direction);

                printf("inside_temperature %.1f\n", (double)buf->_Inside_Temperature/10);
                printf("inside_humidity %d\n",      buf->_Inside_Humidity);
                printf("dew_point_inside %.1f\n",  cTof( getDew(getTemp(buf->_Inside_Temperature), buf->_Inside_Humidity))   );

                printf("rain_rate %.1f\n",          (double)buf->_Rain_Rate*0.01);
                printf("console_battery_voltage %.2f\n",  getVolt(buf->_Console_Battery_Voltage));
                printf("forecast_icone %d\n",      buf->_Forecast_Icons);

                char g[10];
                getGodz(buf->_Time_of_Sunrise, g); printf("time_of_sunrise %s\n", g);
                getGodz(buf->_Time_of_Sunset, g);  printf("time_of_sunset %s\n", g);

                //printf("packet_type %d\n", buf->_PacketType);
                //printf("next_record %d\n", buf->_NextRecord);

            } else {

                //Pares data in Si units
                printf("outside_temperature %.1f\n",getTemp(buf->_Outside_Temperature));
                printf("outside_humidity %d\n",     buf->_Outside_Humidity);
                printf("dew_point_outside %.1f\n",  getDew(getTemp(buf->_Outside_Temperature), buf->_Outside_Humidity));
                printf("barometr %.1f\n",           getBaro(buf->_Barometr));
                printf("bar_trend %d\n", buf->_Bar_Trend);

                printf("10min_avg_wind_speed %.1f\n",getSpeed(buf->_10Min_Avg_Wind_Speed));
                printf("wind_speed %.1f\n",         getSpeed(buf->_Wind_Speed));
                printf("wind_direction %d\n",       buf->_Wind_Direction);

                printf("inside_temperature %.1f\n", getTemp(buf->_Inside_Temperature));
                printf("inside_humidity %d\n",      buf->_Inside_Humidity);
                printf("dew_point_inside %.1f\n", getDew(getTemp(buf->_Inside_Temperature), buf->_Inside_Humidity));

                printf("rain_rate %.1f\n",          getRain(buf->_Rain_Rate));
                printf("console_battery_voltage %.2f\n",  getVolt(buf->_Console_Battery_Voltage));
                printf("forecast_icone %d\n",      buf->_Forecast_Icons);

                char g[10];
                getGodz(buf->_Time_of_Sunrise, g); printf("time_of_sunrise %s\n", g);
                getGodz(buf->_Time_of_Sunset, g);  printf("time_of_sunset %s\n", g);

                //printf("packet_type %d\n", buf->_PacketType);
                //printf("next_record %d\n", buf->_NextRecord);
            }
            //printf("sended_crc %.4x\n", byteSwap(buf->_crc));
            //unsigned short c = crc16_ccitt(bufferRx, sizeof(struct loop1)-2, 0);
            //printf("calculated_crc %.4x \n", c);

         }
    }

    return 0;
}

