cmake_minimum_required(VERSION 3.0)
project(picoDB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH "bin/")

#set(SOURCE_FILES main.cpp)
#add_executable(picoDB ${SOURCE_FILES})

add_subdirectory(./../shared shared_lib)

include_directories(${SHARED_LIB_INCLUDE_DIR})

set(SOURCE_FILES picoServer.cpp
    ../shared/src/Socket.h ../shared/src/Socket.cpp
    ServerSocket.h ServerSocket.cpp)

add_executable(picoServer ${SOURCE_FILES})
target_link_libraries(picoServer shared_lib)
