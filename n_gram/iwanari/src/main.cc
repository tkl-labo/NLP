#include <iostream>
#include <wchar.h>
#include <memory>
#include "stopwatch.h"
#include "n_gram.h"

int main(void)
{
    setlocale(LC_ALL, "ja_JP.UTF-8");

    util::Stopwatch<std::chrono::milliseconds> sw;
    nlp::NGram ngram("../hoge.txt");
    ngram.train(1);
    sw.stop();
    std::cout << "Elapsed: " << sw.showElapsedTime().c_str() << std::endl;

    return 0;
}

