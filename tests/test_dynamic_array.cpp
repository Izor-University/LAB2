#include <gtest/gtest.h>
#include "../core/DynamicArray.hpp"
#include "../core/exceptions.hpp" // Исправлено имя файла

// --- 1. Основные участки предметной логики ---
TEST(DynamicArrayTest, SubjectLogic_SetAndGet) {
    DynamicArray<int> arr(3);
    arr.Set(0, 10);
    arr.Set(1, 20);
    arr.Set(2, 30);

    EXPECT_EQ(arr.GetSize(), 3);
    EXPECT_EQ(arr.Get(1), 20);
}

TEST(DynamicArrayTest, SubjectLogic_Resize) {
    DynamicArray<int> arr(2);
    arr.Set(0, 5);
    arr.Set(1, 15);

    arr.Resize(4); // Увеличение (проверка сохранения данных)
    EXPECT_EQ(arr.GetSize(), 4);
    EXPECT_EQ(arr.Get(1), 15);
    EXPECT_EQ(arr.Get(3), 0); // Проверка инициализации нулями

    arr.Resize(1); // Уменьшение (проверка отсечения)
    EXPECT_EQ(arr.GetSize(), 1);
    EXPECT_EQ(arr.Get(0), 5);
}

TEST(DynamicArrayTest, SubjectLogic_CopyConstructor) {
    DynamicArray<int> arr1(2);
    arr1.Set(0, 100);
    arr1.Set(1, 200);

    DynamicArray<int> arr2(arr1);
    EXPECT_EQ(arr2.GetSize(), 2);
    EXPECT_EQ(arr2.Get(0), 100);

    // Убеждаемся, что это глубокая копия, а не один и тот же указатель
    arr2.Set(0, 999);
    EXPECT_EQ(arr1.Get(0), 100);
}

// --- 2. Охват пограничных ситуаций ---
TEST(DynamicArrayTest, EdgeCases_EmptyArray) {
    // Создание массива нулевой длины (пограничный случай)
    DynamicArray<int> emptyArr(0);
    EXPECT_EQ(emptyArr.GetSize(), 0);

    // Resize из нуля в рабочий размер
    emptyArr.Resize(1);
    emptyArr.Set(0, 99);
    EXPECT_EQ(emptyArr.Get(0), 99);

    // Resize обратно в нуль
    emptyArr.Resize(0);
    EXPECT_EQ(emptyArr.GetSize(), 0);
}

TEST(DynamicArrayTest, EdgeCases_ResizeToSameSize) {
    DynamicArray<int> arr(5);
    arr.Set(0, 42);
    arr.Resize(5); // Размер не меняется
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 42);
}

// --- 3. Некорректные ситуации и сценарии ---
TEST(DynamicArrayTest, IncorrectScenarios_OutOfBounds) {
    DynamicArray<int> arr(2);
    
    // Чтение за пределами
    EXPECT_THROW(arr.Get(-1), IndexOutOfRange);
    EXPECT_THROW(arr.Get(2), IndexOutOfRange);
    
    // Запись за пределами
    EXPECT_THROW(arr.Set(5, 100), IndexOutOfRange);
}

TEST(DynamicArrayTest, IncorrectScenarios_InvalidCreation) {
    // Попытка создать массив отрицательной длины
    EXPECT_THROW(DynamicArray<int> arr(-5), IndexOutOfRange);
}