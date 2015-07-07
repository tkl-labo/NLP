#include <gtest/gtest.h>
#include <stdexcept>
#include "../rpq/nfa.h"

using namespace mgdb;

class NFATest : public ::testing::Test {
    protected:
        BimappingTable mapping_table;
        virtual void SetUp() {
            // add all English alphabet
            for (char alpha = 'a'; alpha <= 'z'; alpha++) {
                std::string string_alpha(1, alpha);
                mapping_table.add(string_alpha);
            }
            
            // add some other samples
            mapping_table.add("abc");
            mapping_table.add("Friend");
            mapping_table.add("Know");
            mapping_table.add("Dislike");
        };
        virtual void TearDown() {};

        void assertThrowWhatEQ(const std::string& regex, const std::string& error_message) {
            NFA* nfa;
            try {
                nfa = new NFA(regex, mapping_table); 
            }
            catch (std::invalid_argument& e) {
                ASSERT_STREQ(e.what(), error_message.c_str());
            }
        }
};

TEST_F(NFATest, simpleTest)
{
    NFA nfa("abc", mapping_table);
    EXPECT_TRUE(nfa.isAccept("abc"));
}

TEST_F(NFATest, alternationTest)
{
    NFA nfa("a|b*", mapping_table);
    EXPECT_TRUE(nfa.isAccept("a"));
    EXPECT_FALSE(nfa.isAccept("cdef"));
    EXPECT_FALSE(nfa.isAccept("aaaaa"));
    EXPECT_TRUE(nfa.isAccept("bbbbb"));
    EXPECT_FALSE(nfa.isAccept("aba"));
}

TEST_F(NFATest, aWordAlphabetTest)
{
    NFA nfa("\"abc\"*", mapping_table);
    EXPECT_TRUE(nfa.isAccept("\"abc\""));
    EXPECT_FALSE(nfa.isAccept("a"));
    EXPECT_FALSE(nfa.isAccept("c"));
    EXPECT_TRUE(nfa.isAccept(""));
    EXPECT_TRUE(nfa.isAccept("\"abc\"\"abc\""));
    EXPECT_FALSE(nfa.isAccept("\"abcabc\""));
}

TEST_F(NFATest, showProgress)
{
    // NFA nfa("a|bc|d|e|a*aab|a*bc", mapping_table);
    // std::list<NFA::State*> current_states = nfa.getStartStates();
    //
    // std::string input = "aaaaaabc";
    //
    // for (auto iter = input.begin(); iter != input.end(); ++iter) {
    //     current_states = nfa.step(current_states, *iter);
    //     cout << "step ( " << *iter << " )" << endl;
    //     for (NFA::State* s : current_states)
    //         cout << "id: " << s->id << endl;
    // }
}

TEST_F(NFATest, invalidSyntaxNoPairedParenthesisAfter)
{
    const std::string regex = "(abac";
    const std::string error_message = "\"(\" has no paired \")\"";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxNoPairedDoubleQuote)
{
    const std::string regex = "\"abac";
    const std::string error_message = "'\"' has no paired '\"'";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxNoPairedParenthesisBefore)
{
    const std::string regex = ")abac";
    const std::string error_message = "\")\" has no paired \"(\"";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxNoAtomBeforeAlternation)
{
    const std::string regex = "|bac";
    const std::string error_message = "No atom before \"|\" operator.";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxNoAtomBeforePlus)
{
    const std::string regex = "+bac";
    const std::string error_message = "No atom before \"+\" operator.";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxNoAtomBeforeAsterisk)
{
    const std::string regex = "*bac";
    const std::string error_message = "No atom before \"*\" operator.";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxNoAtomBeforeQuestionMark)
{
    const std::string regex = "?bac";
    const std::string error_message = "No atom before \"?\" operator.";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxNoAtomInsideParenthesises)
{
    const std::string regex = "bac()";
    const std::string error_message = "No atom inside \"()\" operators.";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxMinusCantBeWrittenBeforeMinus)
{
    const std::string regex = "a--";
    const std::string error_message = "\"-\" can't be written after \"-\"";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxMinusCantBeWrittenAfterNoAlphabet)
{
    const std::string regex = "(a)-";
    const std::string error_message = "\"-\" can't be written after \")\"";
    assertThrowWhatEQ(regex, error_message);
}

TEST_F(NFATest, invalidSyntaxOperatorCantBeWritenBeforeOperator)
{
    const std::string sub_error_message = "\" can't be written after \"";
    char c0[] = {'+', '?', '*'};
    char c1[] = {'+', '?', '*', '-'};
    for (char op0 : c0)
        for (char op1 : c1) {
            std::string it[] = {{op0}, {op1}};
            const std::string regex = "b" + it[0] + it[1] + "ac";
            const std::string error_message = "\"" + it[1] + sub_error_message + it[0] + "\"";
            assertThrowWhatEQ(regex, error_message);
        }
}

