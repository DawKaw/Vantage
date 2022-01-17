**Vantage**

Davis Vantage VUE Weather Station direct connection

This program allows you to download data direct from your Davis Vantage weather station via local network. This program is helpful when we need to create a shell script that is to forward the data to, for example, some database.
The program was tested on Davis Vantage VUE device with ethernet module inserted into base station.

For buld on linux you need:
  - build_essential
  - cmake
  - optionaly QTCreator
  
  For QTCreator just open file CMakeLists.txt, configure and play
  For CMake type:
    cmake .
    make
  
  First run just type:
    ./Vantage -a 192.168.0.127
    
  Of course ip address is the address of your weather station.
  After correct connection you can see: 
 ````
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
