cmake_minimum_required(VERSION 3.29)
project(ping C)

set(CMAKE_C_STANDARD 11)

include_directories(include/)

add_executable(ping
        src/main.c
        src/parser.c
        include/parser.h
        include/socket.h
        src/socket.c
        include/icmp.h
        src/icmp.c
        src/ping.c)
