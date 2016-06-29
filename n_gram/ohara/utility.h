//
// Created by OharaKohei on 2016/06/29.
//

#ifndef CHAP03_UTILITY_H
#define CHAP03_UTILITY_H

#include <iostream>
#include <vector>
#include <map>

//
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

#endif //CHAP03_UTILITY_H
