cmake_minimum_required(VERSION 3.23)
project(CSV_Parser)

set(CMAKE_CXX_STANDARD 17)

add_executable(CSV_Parser main.cpp Parser.h exceptions.h print_t.h)
