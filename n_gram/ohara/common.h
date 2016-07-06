//
// Created by OharaKohei on 2016/07/05.
//

#ifndef CHAP03_COMMON_H
#define CHAP03_COMMON_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

static std::string SENTENCE_END_SYMBOL = "EOS";
static std::string CDR_DELIMITER = "\t";
static char SENTENCE_DELIMITER = ' ';
static std::string NGRAM_START_SYMBOL = "<s>";
static std::string NGRAM_END_SYMBOL = "</s>"; // 未使用

std::vector<std::string> split(const std::string &s, char delim);
typedef std::vector<std::string>::iterator vec_iter;
std::string join(vec_iter first, vec_iter last, char delim);

#endif //CHAP03_COMMON_H
