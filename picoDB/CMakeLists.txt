cmake_minimum_required(VERSION 3.0)
project(picoDB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH "bin/")

#set(SOURCE_FILES main.cpp)
#add_executable(picoDB ${SOURCE_FILES})

add_subdirectory(shared)
add_subdirectory(client)
add_subdirectory(server)