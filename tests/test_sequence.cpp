#include <gtest/gtest.h>
#include "ArraySequence.hpp"
#include "MutableArraySequence.hpp"

TEST(Sequence_TryGet, ValidIndex_HasValue) {
    int d[] = {10, 20, 30};
    MutableArraySequence<int> s(d, 3);
    auto opt = s.TryGet(1);
    EXPECT_TRUE(opt.IsSome());
    EXPECT_EQ(opt.GetValue(), 20);
}

TEST(Sequence_TryGet, InvalidIndex_NoValue) {
    int d[] = {1, 2, 3};
    MutableArraySequence<int> s(d, 3);
    EXPECT_FALSE(s.TryGet(-1).IsSome());
    EXPECT_FALSE(s.TryGet(3).IsSome());
}

TEST(Sequence_TryGetFirst, NonEmpty_HasValue) {
    int d[] = {42, 1, 2};
    MutableArraySequence<int> s(d, 3);
    auto opt = s.TryGetFirst();
    EXPECT_TRUE(opt.IsSome());
    EXPECT_EQ(opt.GetValue(), 42);
}

TEST(Sequence_TryGetFirst, OnEmpty_NoValue) {
    MutableArraySequence<int> s;
    EXPECT_FALSE(s.TryGetFirst().IsSome());
}

TEST(Sequence_TryGetLast, NonEmpty_HasValue) {
    int d[] = {1, 2, 99};
    MutableArraySequence<int> s(d, 3);
    auto opt = s.TryGetLast();
    EXPECT_TRUE(opt.IsSome());
    EXPECT_EQ(opt.GetValue(), 99);
}

TEST(Sequence_TryGetLast, OnEmpty_NoValue) {
    MutableArraySequence<int> s;
    EXPECT_FALSE(s.TryGetLast().IsSome());
}

TEST(Sequence_OperatorBracket, SameAsGet) {
    int d[] = {5, 10, 15};
    MutableArraySequence<int> s(d, 3);
    for (int i = 0; i < 3; i++)
        EXPECT_EQ(s[i], s.Get(i));
}