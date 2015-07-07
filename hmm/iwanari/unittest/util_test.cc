#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "../util.h"

using namespace nlp;

class UtilTest : public ::testing::Test {
    protected:
        virtual void SetUp() {};
        virtual void TearDown() {};
};

TEST_F(UtilTest, joinStringTest)
{
    std::vector<std::string> chars = {"a", "b", "c"};
    EXPECT_STREQ("a,b,c", Util::joinString(chars, ",").c_str());
}

TEST_F(UtilTest, getFirstStringFromStringWithDelimiterTest)
{
    std::string str = "ABC,EFG,HIJ";
    EXPECT_STREQ("ABC", Util::getFirstString(str, ",").c_str());
}

TEST_F(UtilTest, splitStringTest)
{
    std::string str = "ABC,EFG,HIJ";
    std::vector<std::string> ans = {"ABC", "EFG", "HIJ"};
    
    std::vector<std::string> res = Util::splitString(str, ",");
    
    ASSERT_EQ(ans.size(), res.size());
    
    int size = ans.size();
    for (int i = 0; i < size; i++) {
        EXPECT_STREQ(ans.at(i).c_str(), res.at(i).c_str());
    }
}
