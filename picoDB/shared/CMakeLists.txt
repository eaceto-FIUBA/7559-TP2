cmake_minimum_required(VERSION 3.0)
project(picoDB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH "bin/")

#set(SOURCE_FILES main.cpp)
#add_executable(picoDB ${SOURCE_FILES})

set(SHARED_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(SHARED_LIB_INCLUDE_DIR ${SHARED_LIB_INCLUDE_DIR} PARENT_SCOPE)

if(TARGET shared_lib)

message("shared_lib is already defined")

else()

include_directories(${SHARED_LIB_INCLUDE_DIR})

set(LIB_SRCS ./src/person.cpp ./src/SignalHandler.cpp)

add_library(shared_lib STATIC ${LIB_SRCS})

endif()