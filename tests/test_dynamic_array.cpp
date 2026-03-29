#include <gtest/gtest.h>
#include "DynamicArray.hpp"
#include "Exceptions.hpp"

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(DynamicArray_Ctor, Default_ZeroSizeAndCapacity) {
    DynamicArray<int> arr;
    EXPECT_EQ(arr.GetSize(),     0);
    EXPECT_EQ(arr.GetCapacity(), 0);
}

TEST(DynamicArray_Ctor, SizeZero_OK) {
    DynamicArray<int> arr(0);
    EXPECT_EQ(arr.GetSize(),     0);
    EXPECT_EQ(arr.GetCapacity(), 0);
}

TEST(DynamicArray_Ctor, NegativeSize_Throws) {
    EXPECT_THROW(DynamicArray<int>(-1), IndexOutOfRange);
}

TEST(DynamicArray_Ctor, SizeN_ZeroInitialized) {
    DynamicArray<int> arr(5);
    EXPECT_EQ(arr.GetSize(), 5);
    for (int i = 0; i < 5; i++)
        EXPECT_EQ(arr.Get(i), 0);
}

TEST(DynamicArray_Ctor, FromItems_Copies) {
    int data[] = {10, 20, 30};
    DynamicArray<int> arr(data, 3);
    EXPECT_EQ(arr.GetSize(), 3);
    EXPECT_EQ(arr.Get(0), 10);
    EXPECT_EQ(arr.Get(2), 30);
}

TEST(DynamicArray_Ctor, FromItems_NegativeSize_Throws) {
    int data[] = {1, 2};
    EXPECT_THROW(DynamicArray<int>(data, -1), IndexOutOfRange);
}

TEST(DynamicArray_Ctor, Copy_IndependentMemory) {
    int data[] = {1, 2, 3};
    DynamicArray<int> a(data, 3);
    DynamicArray<int> b(a);

    b.Set(0, 999);
    EXPECT_EQ(a.Get(0), 1);
    EXPECT_EQ(b.Get(0), 999);
}

/*******************************************************************
 * operator =
 *******************************************************************/

TEST(DynamicArray_Assign, Basic) {
    int data[] = {5, 6, 7};
    DynamicArray<int> a(data, 3);
    DynamicArray<int> b;
    b = a;
    EXPECT_EQ(b.GetSize(), 3);
    EXPECT_EQ(b.Get(1), 6);
}

TEST(DynamicArray_Assign, IndependentAfterAssign) {
    int data[] = {1, 2, 3};
    DynamicArray<int> a(data, 3);
    DynamicArray<int> b;
    b = a;
    b.Set(0, 42);
    EXPECT_EQ(a.Get(0), 1);
}

/*******************************************************************
 * Get, Set
 *******************************************************************/

TEST(DynamicArray_Access, Get_NegativeIndex_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.Get(-1), IndexOutOfRange);
}

TEST(DynamicArray_Access, Get_IndexEqualSize_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.Get(3), IndexOutOfRange);
}

TEST(DynamicArray_Access, Get_LastValidIndex_OK) {
    int data[] = {10, 20, 30};
    DynamicArray<int> arr(data, 3);
    EXPECT_EQ(arr.Get(2), 30);
}

TEST(DynamicArray_Access, Set_NegativeIndex_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.Set(-1, 0), IndexOutOfRange);
}

TEST(DynamicArray_Access, Set_IndexEqualSize_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.Set(3, 0), IndexOutOfRange);
}

TEST(DynamicArray_Access, Get_OnEmpty_Throws) {
    DynamicArray<int> arr;
    EXPECT_THROW(arr.Get(0), IndexOutOfRange);
}

/*******************************************************************
 * Resize
 *******************************************************************/

TEST(DynamicArray_Resize, NegativeSize_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.Resize(-1), IndexOutOfRange);
}

TEST(DynamicArray_Resize, ToZero_ClearsBuffer) {
    int data[] = {1, 2, 3};
    DynamicArray<int> arr(data, 3);
    arr.Resize(0);
    EXPECT_EQ(arr.GetSize(),     0);
    EXPECT_EQ(arr.GetCapacity(), 3); // Было 0, стало 3 (память не удаляется)
}

TEST(DynamicArray_Resize, BeyondCapacity_Doubles) {
    DynamicArray<int> arr(4);
    arr.Resize(5);
    EXPECT_EQ(arr.GetSize(),     5);
    EXPECT_EQ(arr.GetCapacity(), 8);
}

TEST(DynamicArray_Resize, FarBeyondCapacity_UsesNewSize) {
    DynamicArray<int> arr(2);
    arr.Resize(100);
    EXPECT_EQ(arr.GetCapacity(), 100);
}

/*******************************************************************
 * RemoveAt
 *******************************************************************/

TEST(DynamicArray_RemoveAt, RemoveMiddle) {
    int data[] = {10, 20, 30, 40};
    DynamicArray<int> arr(data, 4);
    arr.RemoveAt(1);
    EXPECT_EQ(arr.GetSize(), 3);
    EXPECT_EQ(arr.Get(1), 30);
}

TEST(DynamicArray_RemoveAt, OutOfRange_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.RemoveAt(-1), IndexOutOfRange);
    EXPECT_THROW(arr.RemoveAt(3), IndexOutOfRange);
}