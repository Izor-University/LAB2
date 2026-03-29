#include <gtest/gtest.h>
#include "LinkedList.hpp"
#include "Exceptions.hpp"

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(LinkedList_Ctor, Default_EmptyList) {
    LinkedList<int> list;
    EXPECT_EQ(list.GetLength(), 0);
}

TEST(LinkedList_Ctor, NegativeSize_Throws) {
    int data[] = {1, 2};
    EXPECT_THROW(LinkedList<int>(data, -1), IndexOutOfRange);
}

TEST(LinkedList_Ctor, FromItems_SizeAndValues) {
    int data[] = {10, 20, 30};
    LinkedList<int> list(data, 3);
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.Get(0), 10);
    EXPECT_EQ(list.Get(2), 30);
}

TEST(LinkedList_Ctor, Copy_IndependentMemory) {
    int data[] = {1, 2, 3};
    LinkedList<int> a(data, 3);
    LinkedList<int> b(a);
    b.Append(999);
    EXPECT_EQ(a.Get(0), 1);
    EXPECT_EQ(a.GetLength(), 3);
    EXPECT_EQ(b.GetLength(), 4);
    EXPECT_EQ(b.Get(3), 999);
}

TEST(LinkedList_Ctor, SingleElement) {
    int data[] = {42};
    LinkedList<int> list(data, 1);
    EXPECT_EQ(list.GetFirst(), 42);
    EXPECT_EQ(list.GetLast(),  42);
}

/*******************************************************************
 * operator =
 *******************************************************************/

TEST(LinkedList_Assign, Basic) {
    int data[] = {1, 2, 3};
    LinkedList<int> a(data, 3);
    LinkedList<int> b;
    b = a;
    EXPECT_EQ(b.GetLength(), 3);
    EXPECT_EQ(b.Get(1), 2);
}

TEST(LinkedList_Assign, IndependentAfterAssign) {
    int data[] = {1, 2, 3};
    LinkedList<int> a(data, 3);
    LinkedList<int> b;
    b = a;
    b.Append(42);
    EXPECT_EQ(a.GetLength(), 3);
    EXPECT_EQ(b.GetLength(), 4);
}

/*******************************************************************
 * GetFirst, GetLast, Get
 *******************************************************************/

TEST(LinkedList_Access, GetFirst_OnEmpty_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.GetFirst(), EmptyCollectionError);
}

TEST(LinkedList_Access, GetLast_OnEmpty_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.GetLast(), EmptyCollectionError);
}

TEST(LinkedList_Access, Get_NegativeIndex_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.Get(-1), IndexOutOfRange);
}

TEST(LinkedList_Access, Get_IndexEqualLength_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.Get(3), IndexOutOfRange);
}

/*******************************************************************
 * Append, Prepend
 *******************************************************************/

TEST(LinkedList_Mutate, Append_Multiple_OrderPreserved) {
    LinkedList<int> list;
    for (int i = 0; i < 5; i++) list.Append(i * 10);
    EXPECT_EQ(list.GetFirst(), 0);
    EXPECT_EQ(list.GetLast(),  40);
    EXPECT_EQ(list.Get(2),     20);
}

TEST(LinkedList_Mutate, Prepend_Multiple_OrderReversed) {
    LinkedList<int> list;
    list.Prepend(3);
    list.Prepend(2);
    list.Prepend(1);
    EXPECT_EQ(list.Get(0), 1);
    EXPECT_EQ(list.Get(1), 2);
    EXPECT_EQ(list.Get(2), 3);
}

/*******************************************************************
 * InsertAt, RemoveAt
 *******************************************************************/

TEST(LinkedList_InsertAt, AtMiddle_ShiftsCorrectly) {
    int data[] = {1, 3, 4};
    LinkedList<int> list(data, 3);
    list.InsertAt(2, 1);
    EXPECT_EQ(list.Get(0), 1);
    EXPECT_EQ(list.Get(1), 2);
    EXPECT_EQ(list.Get(2), 3);
}

TEST(LinkedList_InsertAt, OutOfBounds_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.InsertAt(99, -1), IndexOutOfRange);
    EXPECT_THROW(list.InsertAt(99, 5), IndexOutOfRange);
}

TEST(LinkedList_RemoveAt, RemoveFirst_UpdatesHead) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    list.RemoveAt(0);
    EXPECT_EQ(list.GetFirst(), 2);
    EXPECT_EQ(list.GetLength(), 2);
}

TEST(LinkedList_RemoveAt, OutOfBounds_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.RemoveAt(-1), IndexOutOfRange);
    EXPECT_THROW(list.RemoveAt(3), IndexOutOfRange);
}

/*******************************************************************
 * GetSubList
 *******************************************************************/

TEST(LinkedList_Sublist, FullRange) {
    int data[] = {1, 2, 3, 4, 5};
    LinkedList<int> list(data, 5);
    LinkedList<int>* sub = list.GetSubList(0, 4);
    EXPECT_EQ(sub->GetLength(), 5);
    EXPECT_EQ(sub->Get(0), 1);
    EXPECT_EQ(sub->Get(4), 5);
    delete sub;
}

TEST(LinkedList_Sublist, InvalidRange_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.GetSubList(-1, 2), IndexOutOfRange);
    EXPECT_THROW(list.GetSubList(2, 1), IndexOutOfRange);
    EXPECT_THROW(list.GetSubList(0, 5), IndexOutOfRange);
}

/*******************************************************************
 * Concat
 *******************************************************************/

TEST(LinkedList_Concat, Basic) {
    int d1[] = {1, 2}, d2[] = {3, 4, 5};
    LinkedList<int> a(d1, 2), b(d2, 3);
    LinkedList<int>* res = a.Concat(&b);
    EXPECT_EQ(res->GetLength(), 5);
    EXPECT_EQ(res->Get(0), 1);
    EXPECT_EQ(res->Get(4), 5);
    delete res;
}