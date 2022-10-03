cmake_minimum_required(VERSION 3.22)
project(datasaver)

find_library(PQXX_LIB pqxx REQUIRED)
find_library(PQ_LIB pq REQUIRED)

set(SOURCES main.cpp)

set(CMAKE_CXX_STANDARD 23)

add_executable(datasaver ${SOURCES})

TARGET_LINK_LIBRARIES(datasaver ${PQXX_LIB} ${PQ_LIB})