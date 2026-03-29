#include <gtest/gtest.h>
#include "BitSequence.hpp"

/*******************************************************************
* Helpers
*******************************************************************/

static BitSequence* from_str(const char* s) {
    int n = 0;
    while (s[n]) n++;
    Bit* bits = new Bit[n];
    for (int i = 0; i < n; i++) bits[i] = Bit(s[i] == '1');
    auto* seq = new BitSequence(bits, n);
    delete[] bits;
    return seq;
}

static void expect_bits(const BitSequence* seq, const char* expected) {
    int n = 0;
    while (expected[n]) n++;
    ASSERT_EQ(seq->GetLength(), n);
    for (int i = 0; i < n; i++)
        EXPECT_EQ(seq->Get(i).GetValue(), expected[i] == '1') << "  at index " << i;
}

/*******************************************************************
* And
*******************************************************************/

TEST(BitSequence_And, BasicAnd) {
    BitSequence* a = from_str("1100");
    BitSequence* b = from_str("1010");
    BitSequence* res = a->And(b);
    expect_bits(res, "1000");
    delete a; delete b; delete res;
}

TEST(BitSequence_And, DifferentLengths_PadsLeftZeros) {
    BitSequence* a = from_str("11001");
    BitSequence* b = from_str("110");
    BitSequence* res = a->And(b);
    EXPECT_EQ(res->GetLength(), 5);
    expect_bits(res, "00000");
    delete a; delete b; delete res;
}

/*******************************************************************
* Or
*******************************************************************/

TEST(BitSequence_Or, BasicOr) {
    BitSequence* a = from_str("1100");
    BitSequence* b = from_str("1010");
    BitSequence* res = a->Or(b);
    expect_bits(res, "1110");
    delete a; delete b; delete res;
}

/*******************************************************************
* Xor
*******************************************************************/
TEST(BitSequence_Xor, BasicXor) {
    BitSequence* a = from_str("1100");
    BitSequence* b = from_str("1010");
    BitSequence* res = a->Xor(b);
    expect_bits(res, "0110");
    delete a; delete b; delete res;
}

/*******************************************************************
* Not
*******************************************************************/

TEST(BitSequence_Not, DoubleNot_GivesSelf) {
    BitSequence* a    = from_str("10110100");
    BitSequence* not1 = a->Not();
    BitSequence* not2 = not1->Not();
    expect_bits(not2, "10110100");
    delete a; delete not1; delete not2;
}

/*******************************************************************
* Combined
*******************************************************************/

TEST(BitSequence_Combined, AndWithNot_GivesAndNot) {
    BitSequence* a    = from_str("1111");
    BitSequence* b    = from_str("1010");
    BitSequence* notb = b->Not();
    BitSequence* res  = a->And(notb);
    expect_bits(res, "0101");
    delete a; delete b; delete notb; delete res;
}