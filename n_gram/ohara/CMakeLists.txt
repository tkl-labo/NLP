cmake_minimum_required(VERSION 3.5)
project(chap03)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(cdr2sentence cdr2sentence.cpp common.cpp common.h)
add_executable(sentence2ngram sentence2ngram.cpp common.cpp common.h)
add_executable(perplexity perplexity.cpp common.cpp common.h)