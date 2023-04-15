cmake_minimum_required(VERSION 3.22)
project(datasaver)

find_library(PQXX_LIB pqxx REQUIRED)
find_library(PQ_LIB pq REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES main.cpp structs.h architecture.h command_line.cpp command_line.cpp dataFile.cpp dataBase.cpp)

set(CMAKE_CXX_STANDARD 23)

add_executable(datasaver ${SOURCES})

TARGET_LINK_LIBRARIES(datasaver ${PQXX_LIB} ${PQ_LIB} ${Boost_LIBRARIES})