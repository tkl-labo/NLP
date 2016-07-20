//
// Created by OharaKohei on 2016/07/16.
//

#ifndef HMM_COMMON_H
#define HMM_COMMON_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>

static char TXT_DELIMITER = ' ';
static char SENTENCE_DELIMITER = ' '; // 文中の区切り
static std::string TAG_START_SYMBOL = "<s>";
static std::string TAG_END_SYMBOL = "</s>";


// 以下デバッグ用

// 演算子 << のオーバーロード
// vectorの中身を出力する
// 基本出力: [1, 2, 4,...]
// vectorの中身がvectorであっても以下のように出力する
// [[2, 3,...],[3, 4,...],[2, 1,...],...]
// テンプレートなのでヘッダに定義を書いている
//
template<typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    out << "[";
    size_t last = v.size() - 1;
    for (size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i != last)
            out << ", ";
    }
    out << "]";
    return out;
}

//
// 演算子 << のオーバーロード
// mapの中身を出力する
//
template<typename T1, typename T2>
std::ostream& operator<< (std::ostream& out, const std::map<T1, T2>& map) {
    out << "{";
    for (auto itr = map.begin(); itr != map.end(); ++itr) {
        out << itr->first           // キーを表示
        << ": " << itr->second;    // 値を表示
        if (itr != --map.end()) { // endは末尾の次をさすので、--が必要
            out << ", ";
        }
    }
    out << "}";
    return out;
}

//
// 演算子 << のオーバーロード
// unordered mapの中身を出力する
//
template<typename T1, typename T2>
std::ostream& operator<< (std::ostream& out, const std::unordered_map<T1, T2>& map) {
    out << "{";
    auto end = map.end();
    for (auto itr = map.begin(); itr != end; ++itr) {
        out << itr->first           // キーを表示
        << ": " << itr->second;    // 値を表示
        if (std::next(itr) != end) { // endは末尾の次をさすので、--が必要
            out << ", ";
        }
    }
    out << "}";
    return out;
}

#endif //HMM_COMMON_H
