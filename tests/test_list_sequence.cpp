#include <gtest/gtest.h>
#include "ListSequence.hpp"
#include "MutableListSequence.hpp"
#include "ImmutableListSequence.hpp"
#include "MutableArraySequence.hpp"
#include "Exceptions.hpp"

/*******************************************************************
 * Mutable
 *******************************************************************/

TEST(MutableListSequence, Append_ReturnsSelf) {
    MutableListSequence<int> s;
    Sequence<int>* ret = s.Append(1);
    EXPECT_EQ(ret, &s);
}

TEST(MutableListSequence, Prepend_ReturnsSelf) {
    MutableListSequence<int> s;
    Sequence<int>* ret = s.Prepend(99);
    EXPECT_EQ(ret, &s);
}

TEST(MutableListSequence, InsertAt_MutatesInPlace) {
    int d[] = {1, 3};
    MutableListSequence<int> s(d, 2);
    Sequence<int>* ret = s.InsertAt(2, 1);
    EXPECT_EQ(ret, &s);
    EXPECT_EQ(s.Get(1), 2);
    EXPECT_EQ(s.GetLength(), 3);
}

TEST(MutableListSequence, Map_ReturnsNewObject) {
    int d[] = {1, 2, 3};
    MutableArraySequence<int> s(d, 3);
    Sequence<int>* ret = s.Map([](const int& x) { return x * 10; });

    EXPECT_NE(ret, &s); // Это новый объект!
    EXPECT_EQ(ret->Get(0), 10);
    EXPECT_EQ(s.Get(0), 1); // Оригинал не изменился
    delete ret;
}

/*******************************************************************
 * Immutable
 *******************************************************************/

TEST(ImmutableListSequence, Append_ReturnsNewObject) {
    ImmutableListSequence<int> s;
    Sequence<int>* ret = s.Append(7);
    EXPECT_NE(ret, &s);
    EXPECT_EQ(s.GetLength(), 0);
    EXPECT_EQ(ret->GetLength(), 1);
    EXPECT_EQ(ret->Get(0), 7);
    delete ret;
}

TEST(ImmutableListSequence, Prepend_OriginalUnchanged) {
    int d[] = {2, 3};
    ImmutableListSequence<int> s(d, 2);
    Sequence<int>* ret = s.Prepend(1);
    EXPECT_EQ(s.GetLength(), 2);
    EXPECT_EQ(ret->Get(0), 1);
    EXPECT_EQ(ret->GetLength(), 3);
    delete ret;
}

/*******************************************************************
 * GetSubsequence
 *******************************************************************/

TEST(ListSequence_Subsequence, CorrectValues) {
    int d[] = {10, 20, 30, 40, 50};
    MutableListSequence<int> s(d, 5);
    Sequence<int>* sub = s.GetSubsequence(1, 3);
    EXPECT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 20);
    EXPECT_EQ(sub->Get(2), 40);
    delete sub;
}

TEST(ListSequence_Subsequence, InvalidRange_Throws) {
    int d[] = {10, 20, 30};
    MutableListSequence<int> s(d, 3);
    EXPECT_THROW(s.GetSubsequence(2, 1), IndexOutOfRange);
}

/*******************************************************************
 * Concat
 *******************************************************************/

TEST(ListSequence_Concat, ResultIsNew_OriginalUnchanged) {
    int d1[] = {1, 2}, d2[] = {3, 4};
    MutableListSequence<int> a(d1, 2);
    MutableListSequence<int> b(d2, 2);
    Sequence<int>* res = a.Concat(&b);
    EXPECT_NE(res, &a);
    EXPECT_EQ(a.GetLength(), 2);
    EXPECT_EQ(res->GetLength(), 4);
    EXPECT_EQ(res->Get(0), 1);
    EXPECT_EQ(res->Get(3), 4);
    delete res;
}

TEST(ListSequence_Concat, CrossType_ListWithArray) {
    int d1[] = {1, 2};
    int d2[] = {3, 4};
    MutableListSequence<int> list(d1, 2);
    MutableArraySequence<int> arr(d2, 2);
    Sequence<int>* res = list.Concat(&arr);
    EXPECT_EQ(res->GetLength(), 4);
    EXPECT_EQ(res->Get(2), 3);
    delete res;
}