project(Vantage)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

file(GLOB HEADERS *.h)

set(
 vantage_data.h
 vantage_data.c
 crc16-ccitt.h
)

target_link_libraries(Vantage m)
