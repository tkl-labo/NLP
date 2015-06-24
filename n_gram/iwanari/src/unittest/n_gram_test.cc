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


TEST_F(NGramTest, hasCorrctFileName)
{
    const std::string FILENAME = "hoge.txt";
    NGram ngram(FILENAME);
    ASSERT_STREQ(ngram.inputFile().c_str(),
            FILENAME.c_str());
}
