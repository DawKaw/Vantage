# Vantage

### Davis Vantage VUE Weather Station direct connection

This program allows you to download data direct from your Davis Vantage weather station via local network. This program is helpful when we need to create a shell script that is to forward the data to, for example, some database.
The program was tested on Davis Vantage VUE device with ethernet module inserted into base station.

For build on linux you need:
- build_essential
- cmake
- optionaly QTCreator
  
For QTCreator just open file CMakeLists.txt, configure and click play.<br>
For CMake type:
```bash
#cmake .
#make
````  
First run just type:
  ./Vantage -a 192.168.0.127
    
  Of course ip address is the address of your weather station.
  After correct connection you can see: 
 ```bash
#./Vantage -a 192.168.0.127
outside_temperature 0.9
outside_humidity 52
dew_point_outside -7.9
barometr 1019.9
bar_trend 60
10min_avg_wind_speed 9.7
wind_speed 14.5
wind_direction 304
inside_temperature 23.8
inside_humidity 34
dew_point_inside 7.0
rain_rate 0.0
console_battery_voltage 4.68
forecast_icone 8
time_of_sunrise 7:49
time_of_sunset 16:21
````

#### Other options:
```bash
Usage: ./Vantage -a address [ -p port, -i]
    [-a] device host name or ip address
    [-p] device ip port (default 22222)
    [-i] imperial units
Example:
  ./Vantage -a 192.168.0.127
  ./Vantage -a 192.168.0.127 -p 22222 -i
 ```` 
 
### Useful script
This example simple bash script gets data from weather station end push into your http server as GET request.
```bash
#!/bin/bash

ip="192.168.0.127"

#Get data from weather station (3x try)
myvar=`/usr/local/bin/vantage -a $ip 2>/dev/null`

if [ $? -ne 0 ]; then
    sleep 5
    myvar=`/usr/local/bin/vantage -a $ip 2>/dev/null`
    if [ $? -ne 0 ]; then
        sleep 5
        myvar=`/usr/local/bin/vantage -a $ip`
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi
fi

#Filter value and save into variables
outside_temperature=`grep outside_temperature <<< "$myvar" | awk '{print $2}'`
outside_humidity=`grep outside_humidity <<< "$myvar" | awk '{print $2}'`
dew_point_outside=`grep dew_point_outside <<< "$myvar" | awk '{print $2}'`
barometr=`grep barometr <<< "$myvar" | awk '{print $2}'`
bar_trend=`grep bar_trend <<< "$myvar" | awk '{print $2}'`
X10min_avg_wind_speed=`grep 10min_avg_wind_speed <<< "$myvar" | awk '{print $2}'`
wind_speed=`grep wind_speed <<< "$myvar" | awk '{print $2}'`
wind_direction=`grep wind_direction <<< "$myvar" | awk '{print $2}'`
inside_temperature=`grep inside_temperature <<< "$myvar" | awk '{print $2}'`
inside_humidity=`grep inside_humidity <<< "$myvar" | awk '{print $2}'`
dew_point_inside=`grep dew_point_inside <<< "$myvar" | awk '{print $2}'`
rain_rate=`grep rain_rate <<< "$myvar" | awk '{print $2}'`
console_battery_voltage=`grep console_battery_voltage <<< "$myvar" | awk '{print $2}'`
forecast_icone=`grep forecast_icone <<< "$myvar" | awk '{print $2}'`
time_of_sunrise=`grep time_of_sunrise <<< "$myvar" | awk '{print $2}'`
time_of_sunset=`grep time_of_sunset <<< "$myvar" | awk '{print $2}'`

#Test variables - remove "#" if you need.
#echo "$outside_temperature"
#echo "$outside_humidity"
#echo "$dew_point_outside"
#echo "$barometr"
#echo "$bar_trend"
#echo "$X10min_avg_wind_speed"
#echo "$wind_speed"
#echo "$wind_direction"
#echo "$inside_temperature"
#echo "$inside_humidity"
#echo "$dew_point_inside"
#echo "$rain_rate"
#echo "$console_battery_voltage"
#echo "$forecast_icone"
#echo "$time_of_sunrise"
#echo "$time_of_sunset"

#Round wind do integer
wind2=`echo "$X10min_avg_wind_speed"| xargs printf '%.0f'`
#echo "$wind2"

#Check collected wind average
if (($wind2>150)); then
    echo "Wind read failed. >150km/h: [$wind2]";
    exit 0;
fi

#Assemby url to API as GET request
get="http://api.yourwebserver.com/termo/collect.php?"
get+="tempV=$outside_temperature&"
get+="humV=$outside_humidity&"
get+="dewV=$dew_point_outside&"
get+="press=$barometr&"
get+="wind=$wind2&"
get+="winddir=$wind_direction&"
get+="rain=$rain_rate&"
get+="id=1500"

#echo $get
#push GET request to server
curl -s "$get">/dev/null

````

This script can run from **crone** on every 5 minutes, for instatnce: ````*/5 * * * * /usr/local/bin/weather.sh````
