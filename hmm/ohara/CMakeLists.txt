cmake_minimum_required(VERSION 3.5)
project(hmm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(txt2tagseq txt2tagseq.cpp common.cpp common.h)
add_executable(tagseq2tagseq_count tagseq2tagseq_count.cpp common.cpp common.h)
add_executable(txt2tagword_count txt2tagword_count.cpp common.cpp common.h)
add_executable(viterbi viterbi.cpp common.cpp common.h)
add_executable(forward forward.cpp common.cpp common.h)
