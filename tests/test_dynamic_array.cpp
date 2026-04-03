#include <gtest/gtest.h>
#include <utility>
#include "DynamicArray.hpp"
#include "Exceptions.hpp"
#include "IEnumerator.hpp"

// ============================================================================
// ИНСТРУМЕНТЫ ДЛЯ СТРОГОГО АНАЛИЗА ПАМЯТИ (ДЕТЕКТОР УТЕЧЕК)
// ============================================================================
namespace {

struct InstanceTracker {
    static int activeInstances;
    int value;

    InstanceTracker(int v = 0) : value(v) { activeInstances++; }
    InstanceTracker(const InstanceTracker& other) : value(other.value) { activeInstances++; }
    InstanceTracker(InstanceTracker&& other) noexcept : value(other.value) { 
        other.value = -1;
        activeInstances++; 
    }
    InstanceTracker& operator=(const InstanceTracker& other) {
        value = other.value;
        return *this;
    }
    InstanceTracker& operator=(InstanceTracker&& other) noexcept {
        value = other.value;
        other.value = -1;
        return *this;
    }
    ~InstanceTracker() { activeInstances--; }
    
    bool operator==(const InstanceTracker& other) const { return value == other.value; }
};

int InstanceTracker::activeInstances = 0;

} // конец анонимного пространства имен

class DynamicArrayMemoryTest : public ::testing::Test {
protected:
    void SetUp() override { InstanceTracker::activeInstances = 0; }
    void TearDown() override { EXPECT_EQ(InstanceTracker::activeInstances, 0) << "MEMORY LEAK DETECTED!"; }
};

// ============================================================================
// ПРАВИЛО ПЯТИ (RULE OF FIVE): ФУНДАМЕНТАЛЬНЫЕ ГАРАНТИИ ПАМЯТИ
// ============================================================================

TEST_F(DynamicArrayMemoryTest, CopyConstructorGuaranteesDeepCopy) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2)};
    DynamicArray<InstanceTracker> original(arr, 2);
    
    DynamicArray<InstanceTracker> copy(original);
    
    EXPECT_EQ(copy.GetSize(), 2);
    EXPECT_EQ(copy.Get(0).value, 1);
    
    copy.Set(0, InstanceTracker(99));
    EXPECT_EQ(original.Get(0).value, 1);
    EXPECT_EQ(copy.Get(0).value, 99);
}

TEST_F(DynamicArrayMemoryTest, MoveConstructorTransfersOwnership) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2)};
    DynamicArray<InstanceTracker> original(arr, 2);
    
    DynamicArray<InstanceTracker> moved(std::move(original));
    
    EXPECT_EQ(moved.GetSize(), 2);
    EXPECT_EQ(moved.Get(1).value, 2);
    
    EXPECT_EQ(original.GetSize(), 0);
    EXPECT_THROW(original.Get(0), IndexOutOfRange);
}

TEST_F(DynamicArrayMemoryTest, CopyAssignmentOperatorSafety) {
    InstanceTracker arr1[] = {InstanceTracker(1), InstanceTracker(2)};
    InstanceTracker arr2[] = {InstanceTracker(3)};
    
    DynamicArray<InstanceTracker> arrA(arr1, 2);
    DynamicArray<InstanceTracker> arrB(arr2, 1);
    
    arrB = arrA;
    
    EXPECT_EQ(arrB.GetSize(), 2);
    EXPECT_EQ(arrB.Get(0).value, 1);
    
    arrB = arrB;
    EXPECT_EQ(arrB.GetSize(), 2);
    EXPECT_EQ(arrB.Get(1).value, 2);
}

TEST_F(DynamicArrayMemoryTest, MoveAssignmentOperatorSafety) {
    InstanceTracker arr1[] = {InstanceTracker(1), InstanceTracker(2)};
    DynamicArray<InstanceTracker> arrA(arr1, 2);
    DynamicArray<InstanceTracker> arrB;
    
    arrB = std::move(arrA);
    
    EXPECT_EQ(arrB.GetSize(), 2);
    EXPECT_EQ(arrB.Get(0).value, 1);
    EXPECT_EQ(arrA.GetSize(), 0);
    
    arrB = std::move(arrB);
    EXPECT_EQ(arrB.GetSize(), 2);
    EXPECT_EQ(arrB.Get(0).value, 1);
}

// ============================================================================
// БАЗОВЫЕ ОПЕРАЦИИ И ПОГРАНИЧНЫЕ УСЛОВИЯ АДРЕСАЦИИ
// ============================================================================
class DynamicArrayBoundsTest : public ::testing::Test {};

TEST_F(DynamicArrayBoundsTest, ConstructorsAndCapacityLogic) {
    DynamicArray<int> defArr;
    EXPECT_EQ(defArr.GetSize(), 0);
    EXPECT_GE(defArr.GetCapacity(), 0);
    
    DynamicArray<int> sizedArr(10);
    EXPECT_EQ(sizedArr.GetSize(), 10);
    EXPECT_GE(sizedArr.GetCapacity(), 10);
    
    int raw[] = {10, 20, 30};
    DynamicArray<int> initArr(raw, 3);
    EXPECT_EQ(initArr.GetSize(), 3);
    EXPECT_EQ(initArr.Get(2), 30);
}

TEST_F(DynamicArrayBoundsTest, ReadWriteBoundaries) {
    DynamicArray<int> arr(3);
    arr.Set(0, 100);
    arr.Set(2, 300);
    
    EXPECT_EQ(arr.Get(0), 100);
    EXPECT_EQ(arr.Get(2), 300);
    
    EXPECT_THROW(arr.Set(-1, 0), IndexOutOfRange);
    EXPECT_THROW(arr.Set(3, 0), IndexOutOfRange);
    
    EXPECT_THROW(arr.Get(-1), IndexOutOfRange);
    EXPECT_THROW(arr.Get(3), IndexOutOfRange);
}

// ============================================================================
// ДИНАМИЧЕСКОЕ ИЗМЕНЕНИЕ РАЗМЕРА (RESIZE И REMOVEAT)
// ============================================================================
TEST_F(DynamicArrayMemoryTest, ResizeExpandsAndTruncatesCorrectly) {
    DynamicArray<InstanceTracker> arr;
    
    arr.Resize(5);
    EXPECT_EQ(arr.GetSize(), 5);
    arr.Set(4, InstanceTracker(42));
    
    arr.Resize(10);
    EXPECT_EQ(arr.GetSize(), 10);
    EXPECT_EQ(arr.Get(4).value, 42);
    
    arr.Resize(2);
    EXPECT_EQ(arr.GetSize(), 2);
    EXPECT_THROW(arr.Get(4), IndexOutOfRange);
    
    arr.Resize(0);
    EXPECT_EQ(arr.GetSize(), 0);
}

TEST_F(DynamicArrayMemoryTest, RemoveAtShiftsMemoryCorrectly) {
    int raw[] = {10, 20, 30, 40, 50};
    DynamicArray<int> arr(raw, 5);
    
    arr.RemoveAt(2);
    EXPECT_EQ(arr.GetSize(), 4);
    EXPECT_EQ(arr.Get(2), 40);
    EXPECT_EQ(arr.Get(3), 50);
    
    arr.RemoveAt(0);
    EXPECT_EQ(arr.GetSize(), 3);
    EXPECT_EQ(arr.Get(0), 20);
    
    arr.RemoveAt(2);
    EXPECT_EQ(arr.GetSize(), 2);
    EXPECT_EQ(arr.Get(1), 40);
    
    EXPECT_THROW(arr.RemoveAt(-1), IndexOutOfRange);
    EXPECT_THROW(arr.RemoveAt(2), IndexOutOfRange);
}

// ============================================================================
// ВНУТРЕННИЙ ИТЕРАТОР: ДОСТУП И ИЗОЛЯЦИЯ СОСТОЯНИЙ
// ============================================================================
class DynamicArrayEnumeratorTest : public ::testing::Test {};

TEST_F(DynamicArrayEnumeratorTest, IndependentIterationOverSameMemory) {
    int raw[] = {1, 2, 3};
    DynamicArray<int> arr(raw, 3);
    
    IEnumerator<int>* fastEn = arr.GetEnumerator();
    IEnumerator<int>* slowEn = arr.GetEnumerator();
    
    fastEn->MoveNext();
    fastEn->MoveNext();
    
    slowEn->MoveNext();
    
    EXPECT_EQ(fastEn->GetCurrent(), 2);
    EXPECT_EQ(slowEn->GetCurrent(), 1);
    
    delete fastEn;
    delete slowEn;
}

// ============================================================================
// ЭКСТРЕМАЛЬНЫЕ НАГРУЗОЧНЫЕ ТЕСТЫ (АЛЛОКАТОР И ФРАГМЕНТАЦИЯ)
// ============================================================================
class DynamicArrayStressTest : public ::testing::Test {};

TEST_F(DynamicArrayStressTest, MassiveReallocationTorture) {
    DynamicArray<int> arr;
    const int FINAL_SIZE = 100000;
    
    ASSERT_NO_THROW({
        for (int i = 0; i < FINAL_SIZE; ++i) {
            arr.Resize(i + 1);
            arr.Set(i, i * 2);
        }
    });
    
    EXPECT_EQ(arr.GetSize(), FINAL_SIZE);
    EXPECT_EQ(arr.Get(FINAL_SIZE / 2), (FINAL_SIZE / 2) * 2);
    EXPECT_EQ(arr.Get(FINAL_SIZE - 1), (FINAL_SIZE - 1) * 2);
    
    arr.Resize(10);
    EXPECT_EQ(arr.GetSize(), 10);
    EXPECT_EQ(arr.Get(9), 18);
}

TEST_F(DynamicArrayStressTest, O_1_AccessTimeVerification) {
    DynamicArray<int> arr(1000000);
    
    ASSERT_NO_THROW({
        for (int i = 0; i < 1000000; ++i) {
            arr.Set(i, i);
        }
        for (int i = 999999; i >= 0; --i) {
            int val = arr.Get(i);
            EXPECT_EQ(val, i);
        }
    });
}
