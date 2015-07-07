#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "../n_gram.h"

using namespace nlp;


class NGramTest : public ::testing::Test {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};


TEST_F(NGramTest, hasCorrctN)
{
    // const std::string FILENAME = "hoge.txt";
    // const int N = 2;
    // NGram ngram(N);
    //
    // ASSERT_EQ(ngram.N(), N);
}
