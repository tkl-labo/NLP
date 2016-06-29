cmake_minimum_required(VERSION 3.5)
project(chap03)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp ngram.cpp ngram.h utility.cpp utility.h)
add_executable(chap03 ${SOURCE_FILES})