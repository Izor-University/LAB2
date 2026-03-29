#include <gtest/gtest.h>
#include "ArraySequence.hpp"
#include "MutableArraySequence.hpp"
#include "ImmutableArraySequence.hpp"
#include "Exceptions.hpp"

/*******************************************************************
 * Mutable
 *******************************************************************/

TEST(MutableArraySequence, Append_ReturnsSelf) {
    MutableArraySequence<int> s;
    Sequence<int>* ret = s.Append(1);
    EXPECT_EQ(ret, &s);
}

TEST(MutableArraySequence, Prepend_ReturnsSelf) {
    MutableArraySequence<int> s;
    Sequence<int>* ret = s.Prepend(1);
    EXPECT_EQ(ret, &s);
}

TEST(MutableArraySequence, InsertAt_ReturnsSelf) {
    MutableArraySequence<int> s;
    s.Append(1); s.Append(3);
    Sequence<int>* ret = s.InsertAt(2, 1);
    EXPECT_EQ(ret, &s);
    EXPECT_EQ(s.Get(1), 2);
}

TEST(MutableArraySequence, Map_ReturnsNewObject) {
    int d[] = {1, 2, 3};
    MutableArraySequence<int> s(d, 3);
    Sequence<int>* ret = s.Map([](const int& x) { return x * 10; });

    EXPECT_NE(ret, &s); // Это новый объект!
    EXPECT_EQ(ret->Get(0), 10);
    EXPECT_EQ(s.Get(0), 1); // Оригинал не изменился
    delete ret;
}

TEST(MutableArraySequence, Where_ReturnsNewObject) {
    int d[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> s(d, 5);
    Sequence<int>* ret = s.Where([](const int& x) { return x > 3; });

    EXPECT_NE(ret, &s);
    EXPECT_EQ(ret->GetLength(), 2);
    EXPECT_EQ(ret->Get(0), 4);
    EXPECT_EQ(s.GetLength(), 5); // Оригинал остался равен 5
    delete ret;
}

TEST(MutableArraySequence, Slice_ReturnsNewObject) {
    int d1[] = {1, 2, 3, 4, 5};
    int d2[] = {99, 88};
    MutableArraySequence<int> s(d1, 5);
    MutableArraySequence<int> a(d2, 2);
    Sequence<int>* ret = s.Slice(1, 2, &a);

    EXPECT_NE(ret, &s);
    EXPECT_EQ(ret->Get(1), 99);
    EXPECT_EQ(s.Get(1), 2); // Оригинал не изменился
    delete ret;
}

TEST(MutableArraySequence, InsertAt_BeyondSize_Throws) {
    MutableArraySequence<int> s;
    EXPECT_THROW(s.InsertAt(1, 1), IndexOutOfRange);
}

/*******************************************************************
 * Immutable
 *******************************************************************/

TEST(ImmutableArraySequence, Append_ReturnsNewObject) {
    ImmutableArraySequence<int> s;
    Sequence<int>* ret = s.Append(42);
    EXPECT_NE(ret, &s);
    EXPECT_EQ(s.GetLength(), 0);
    EXPECT_EQ(ret->GetLength(), 1);
    EXPECT_EQ(ret->Get(0), 42);
    delete ret;
}

TEST(ImmutableArraySequence, Map_OriginalUnchanged) {
    int d[] = {1, 2, 3};
    ImmutableArraySequence<int> s(d, 3);
    Sequence<int>* ret = s.Map([](const int& x) { return x * 2; });
    EXPECT_EQ(s.Get(0), 1);
    EXPECT_EQ(ret->Get(0), 2);
    delete ret;
}

/*******************************************************************
* Reduce
*******************************************************************/

TEST(ArraySequence_Reduce, Sum) {
    int d[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> s(d, 5);
    int res = s.Reduce([](const int& a, const int& b) { return a + b; }, 0);
    EXPECT_EQ(res, 15);
}

/*******************************************************************
 * GetFirst, GetLast, GetSubsequence
 *******************************************************************/

TEST(ArraySequence_GetFirstLast, Empty_Throws) {
    MutableArraySequence<int> s;
    EXPECT_THROW(s.GetFirst(), EmptyCollectionError);
    EXPECT_THROW(s.GetLast(), EmptyCollectionError);
}

TEST(ArraySequence_Subsequence, MutableReturnsNew) {
    int d[] = {10, 20, 30, 40, 50};
    MutableArraySequence<int> s(d, 5);
    Sequence<int>* sub = s.GetSubsequence(1, 3);
    EXPECT_NE(sub, &s);
    EXPECT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 20);
    delete sub;
}

TEST(ArraySequence_Subsequence, InvalidRange_Throws) {
    int d[] = {10, 20, 30};
    MutableArraySequence<int> s(d, 3);
    EXPECT_THROW(s.GetSubsequence(2, 1), IndexOutOfRange);
    EXPECT_THROW(s.GetSubsequence(-1, 2), IndexOutOfRange);
}

/*******************************************************************
 * Concat
 *******************************************************************/

TEST(ArraySequence_Concat, ResultIsNew_OriginalUnchanged) {
    int d1[] = {1, 2}, d2[] = {3, 4, 5};
    MutableArraySequence<int> a(d1, 2);
    MutableArraySequence<int> b(d2, 3);
    Sequence<int>* res = a.Concat(&b);
    EXPECT_NE(res, &a);
    EXPECT_EQ(a.GetLength(), 2);
    EXPECT_EQ(res->GetLength(), 5);
    delete res;
}

/*******************************************************************
 * Enumerator
 *******************************************************************/

TEST(MutableArraySequence, Enumerator_IteratesAll) {
    int d[] = {5, 10, 15};
    MutableArraySequence<int> s(d, 3);
    IEnumerator<int>* it = s.GetEnumerator();
    int sum = 0;
    while (it->MoveNext()) sum += it->GetCurrent();
    delete it;
    EXPECT_EQ(sum, 30);
}