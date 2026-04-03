#include <gtest/gtest.h>
#include "MutableArraySequence.hpp"
#include "ImmutableArraySequence.hpp"
#include "Exceptions.hpp"
#include "IEnumerator.hpp"

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ И ПРЕДИКАТЫ
// ============================================================================
int MultiplyByTwo(const int& x) { return x * 2; }
bool IsOdd(const int& x) { return x % 2 != 0; }
int AccumulateSum(const int& acc, const int& cur) { return acc + cur; }

// ============================================================================
// MUTABLE ARRAY SEQUENCE: БАЗОВЫЕ ОПЕРАЦИИ И УПРАВЛЕНИЕ ПАМЯТЬЮ
// ============================================================================
class MutableArraySequenceTest : public ::testing::Test {};

TEST_F(MutableArraySequenceTest, DefaultConstructorState) {
    MutableArraySequence<int> seq;
    EXPECT_EQ(seq.GetLength(), 0);
    EXPECT_THROW(seq.GetFirst(), EmptyCollectionError);
    EXPECT_THROW(seq.GetLast(), EmptyCollectionError);
    EXPECT_THROW(seq.Get(0), IndexOutOfRange);
}

TEST_F(MutableArraySequenceTest, ArrayConstructorAndDeepCopy) {
    int arr[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> original(arr, 5);
    
    EXPECT_EQ(original.GetLength(), 5);
    EXPECT_EQ(original.GetFirst(), 1);
    EXPECT_EQ(original.GetLast(), 5);

    // Конструктор копирования
    MutableArraySequence<int> copy(original);
    EXPECT_EQ(copy.GetLength(), 5);
    
    // Проверка независимости памяти (Косвенная мутация)
    // Добавляем элемент в начало и в конец копии
    copy.Prepend(999); 
    copy.Append(6);
    
    // Оригинал должен остаться абсолютно нетронутым
    EXPECT_EQ(original.GetLength(), 5);
    EXPECT_EQ(original.GetFirst(), 1);
    EXPECT_EQ(original.GetLast(), 5);
    
    // Копия должна измениться
    EXPECT_EQ(copy.GetLength(), 7);
    EXPECT_EQ(copy.GetFirst(), 999);
    EXPECT_EQ(copy.GetLast(), 6);
}

TEST_F(MutableArraySequenceTest, BoundaryElementAccess) {
    int arr[] = {42};
    MutableArraySequence<int> seq(arr, 1);
    
    EXPECT_EQ(seq.GetFirst(), 42);
    EXPECT_EQ(seq.GetLast(), 42);
    EXPECT_EQ(seq.Get(0), 42);
    
    EXPECT_THROW(seq.Get(-1), IndexOutOfRange);
    EXPECT_THROW(seq.Get(1), IndexOutOfRange);
}

// ============================================================================
// MUTABLE ARRAY SEQUENCE: МУТАЦИИ И STRONG EXCEPTION GUARANTEE
// ============================================================================
TEST_F(MutableArraySequenceTest, InPlaceMutationsAndPointers) {
    MutableArraySequence<int> seq;
    
    Sequence<int>* ptr1 = seq.Append(10);
    EXPECT_EQ(ptr1, &seq); // Должен возвращать this
    EXPECT_EQ(seq.GetLength(), 1);
    EXPECT_EQ(seq.GetFirst(), 10);

    Sequence<int>* ptr2 = seq.Prepend(5);
    EXPECT_EQ(ptr2, &seq);
    EXPECT_EQ(seq.GetFirst(), 5);
    EXPECT_EQ(seq.GetLast(), 10);

    Sequence<int>* ptr3 = seq.InsertAt(7, 1);
    EXPECT_EQ(ptr3, &seq);
    EXPECT_EQ(seq.Get(1), 7);
    EXPECT_EQ(seq.GetLength(), 3);
}

TEST_F(MutableArraySequenceTest, InsertAtBoundaries) {
    MutableArraySequence<int> seq;
    seq.InsertAt(1, 0); // Вставка в пустой
    EXPECT_EQ(seq.GetFirst(), 1);
    
    seq.InsertAt(2, 1); // Вставка в конец (эквивалент Append)
    EXPECT_EQ(seq.GetLast(), 2);
    
    seq.InsertAt(0, 0); // Вставка в начало (эквивалент Prepend)
    EXPECT_EQ(seq.GetFirst(), 0);
    EXPECT_EQ(seq.GetLength(), 3);
}

TEST_F(MutableArraySequenceTest, StrongExceptionGuaranteeOnModifiers) {
    int arr[] = {1, 2, 3};
    MutableArraySequence<int> seq(arr, 3);
    
    // Состояние до ошибки
    int originalLength = seq.GetLength();
    int originalFirst = seq.GetFirst();
    int originalLast = seq.GetLast();

    // Некорректные индексы
    EXPECT_THROW(seq.InsertAt(99, -1), IndexOutOfRange);
    EXPECT_THROW(seq.InsertAt(99, 4), IndexOutOfRange); // index > length
    
    // Состояние коллекции не должно измениться после выброса исключения
    EXPECT_EQ(seq.GetLength(), originalLength);
    EXPECT_EQ(seq.GetFirst(), originalFirst);
    EXPECT_EQ(seq.GetLast(), originalLast);
}

// ============================================================================
// IMMUTABLE ARRAY SEQUENCE: СТРОГАЯ ИЗОЛЯЦИЯ И ВОЗВРАЩАЕМЫЕ ЗНАЧЕНИЯ
// ============================================================================
class ImmutableArraySequenceTest : public ::testing::Test {};

TEST_F(ImmutableArraySequenceTest, StrictImmutabilityOnAppendPrepend) {
    int arr[] = {10, 20};
    ImmutableArraySequence<int> original(arr, 2);
    
    Sequence<int>* modified1 = original.Append(30);
    Sequence<int>* modified2 = original.Prepend(0);
    
    // Проверка создания новых объектов
    EXPECT_NE(modified1, &original);
    EXPECT_NE(modified2, &original);
    EXPECT_NE(modified1, modified2);
    
    // Проверка оригинального объекта (не должен измениться)
    EXPECT_EQ(original.GetLength(), 2);
    EXPECT_EQ(original.GetFirst(), 10);
    EXPECT_EQ(original.GetLast(), 20);
    
    // Проверка новых объектов
    EXPECT_EQ(modified1->GetLength(), 3);
    EXPECT_EQ(modified1->GetLast(), 30);
    
    EXPECT_EQ(modified2->GetLength(), 3);
    EXPECT_EQ(modified2->GetFirst(), 0);
    
    delete modified1;
    delete modified2;
}

TEST_F(ImmutableArraySequenceTest, StrictImmutabilityOnInsert) {
    int arr[] = {1, 3};
    ImmutableArraySequence<int> original(arr, 2);
    
    Sequence<int>* modified = original.InsertAt(2, 1);
    
    EXPECT_EQ(original.GetLength(), 2);
    EXPECT_EQ(original.Get(1), 3); // Оригинал не сдвинут
    
    EXPECT_EQ(modified->GetLength(), 3);
    EXPECT_EQ(modified->Get(1), 2);
    EXPECT_EQ(modified->Get(2), 3); // Элементы сдвинуты в копии
    
    delete modified;
}

// ============================================================================
// ENUMERATOR: КОНЕЧНЫЙ АВТОМАТ И БЕЗОПАСНОСТЬ ДОСТУПА К ПАМЯТИ
// ============================================================================
class ArrayEnumeratorTest : public ::testing::Test {};

TEST_F(ArrayEnumeratorTest, EmptySequenceEnumeratorState) {
    MutableArraySequence<int> seq;
    IEnumerator<int>* en = seq.GetEnumerator();
    
    EXPECT_THROW(en->GetCurrent(), IndexOutOfRange);
    EXPECT_FALSE(en->MoveNext());
    EXPECT_THROW(en->GetCurrent(), IndexOutOfRange);
    
    delete en;
}

TEST_F(ArrayEnumeratorTest, MultipleIndependentEnumerators) {
    int arr[] = {1, 2, 3};
    MutableArraySequence<int> seq(arr, 3);
    
    IEnumerator<int>* en1 = seq.GetEnumerator();
    IEnumerator<int>* en2 = seq.GetEnumerator();
    
    EXPECT_TRUE(en1->MoveNext());
    EXPECT_TRUE(en1->MoveNext()); // en1 на индексе 1 (значение 2)
    
    EXPECT_TRUE(en2->MoveNext()); // en2 на индексе 0 (значение 1)
    
    EXPECT_EQ(en1->GetCurrent(), 2);
    EXPECT_EQ(en2->GetCurrent(), 1);
    
    delete en1;
    delete en2;
}

// ============================================================================
// АЛГОРИТМЫ ЯДРА (MAP, WHERE, REDUCE, CONCAT, GETSUBSEQUENCE)
// ============================================================================
class ArrayAlgorithmsTest : public ::testing::Test {};

TEST_F(ArrayAlgorithmsTest, MapCorrectnessAndMemory) {
    int arr[] = {1, 2, 3};
    MutableArraySequence<int> seq(arr, 3);
    
    Sequence<int>* result = seq.Map(MultiplyByTwo);
    
    EXPECT_NE(result, &seq); // Всегда новая последовательность
    EXPECT_EQ(result->GetLength(), 3);
    EXPECT_EQ(result->Get(0), 2);
    EXPECT_EQ(result->Get(1), 4);
    EXPECT_EQ(result->Get(2), 6);
    
    delete result;
}

TEST_F(ArrayAlgorithmsTest, WhereFilteringLogic) {
    int arr[] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> seq(arr, 5);
    
    Sequence<int>* filtered = seq.Where(IsOdd);
    
    EXPECT_EQ(filtered->GetLength(), 3);
    EXPECT_EQ(filtered->Get(0), 1);
    EXPECT_EQ(filtered->Get(1), 3);
    EXPECT_EQ(filtered->Get(2), 5);
    
    delete filtered;
}

TEST_F(ArrayAlgorithmsTest, WhereReturnsEmpty) {
    int arr[] = {2, 4, 6};
    MutableArraySequence<int> seq(arr, 3);
    
    Sequence<int>* filtered = seq.Where(IsOdd); // Нет нечетных
    
    EXPECT_EQ(filtered->GetLength(), 0);
    EXPECT_THROW(filtered->GetFirst(), EmptyCollectionError);
    
    delete filtered;
}

TEST_F(ArrayAlgorithmsTest, ReduceAccumulation) {
    int arr[] = {10, 20, 30};
    MutableArraySequence<int> seq(arr, 3);
    
    int result = seq.Reduce(AccumulateSum, 0);
    EXPECT_EQ(result, 60);
    
    int resultWithBase = seq.Reduce(AccumulateSum, 100);
    EXPECT_EQ(resultWithBase, 160);
}

TEST_F(ArrayAlgorithmsTest, ConcatEdgeCases) {
    int arr1[] = {1, 2};
    int arr2[] = {3, 4};
    MutableArraySequence<int> seq1(arr1, 2);
    MutableArraySequence<int> seq2(arr2, 2);
    MutableArraySequence<int> emptySeq;
    
    Sequence<int>* result1 = seq1.Concat(&seq2);
    EXPECT_EQ(result1->GetLength(), 4);
    EXPECT_EQ(result1->Get(2), 3);
    
    // Concat с пустой
    Sequence<int>* result2 = seq1.Concat(&emptySeq);
    EXPECT_EQ(result2->GetLength(), 2);
    EXPECT_EQ(result2->GetLast(), 2);
    
    // Concat с nullptr
    Sequence<int>* result3 = seq1.Concat(nullptr);
    EXPECT_EQ(result3->GetLength(), 2);
    
    delete result1;
    delete result2;
    delete result3;
}

TEST_F(ArrayAlgorithmsTest, SubsequenceValidationAndExceptions) {
    int arr[] = {0, 10, 20, 30, 40};
    MutableArraySequence<int> seq(arr, 5);
    
    Sequence<int>* sub1 = seq.GetSubsequence(1, 3);
    EXPECT_EQ(sub1->GetLength(), 3);
    EXPECT_EQ(sub1->GetFirst(), 10);
    EXPECT_EQ(sub1->GetLast(), 30);
    
    // Некорректные индексы
    EXPECT_THROW(seq.GetSubsequence(-1, 2), IndexOutOfRange);
    EXPECT_THROW(seq.GetSubsequence(1, 5), IndexOutOfRange); // endIndex >= length
    EXPECT_THROW(seq.GetSubsequence(3, 1), IndexOutOfRange); // startIndex > endIndex
    
    delete sub1;
}

// ============================================================================
// НАГРУЗОЧНЫЕ И СТРЕСС-ТЕСТЫ (ВЫЯВЛЕНИЕ УТЕЧЕК И ПАДЕНИЙ ПРОИЗВОДИТЕЛЬНОСТИ)
// ============================================================================
class ArrayStressTest : public ::testing::Test {};

TEST_F(ArrayStressTest, MassiveAllocationsAndReallocations) {
    MutableArraySequence<int> seq;
    const int ITERATIONS = 100000;
    
    ASSERT_NO_THROW({
        for (int i = 0; i < ITERATIONS; ++i) {
            seq.Append(i);
        }
    });
    
    EXPECT_EQ(seq.GetLength(), ITERATIONS);
    EXPECT_EQ(seq.GetFirst(), 0);
    EXPECT_EQ(seq.GetLast(), ITERATIONS - 1);
    EXPECT_EQ(seq.Get(ITERATIONS / 2), ITERATIONS / 2);
    
    // Тестирование масштабирования при вставке в начало
    ASSERT_NO_THROW({
        for (int i = 0; i < 500; ++i) {
            seq.Prepend(-i);
        }
    });
    
    EXPECT_EQ(seq.GetLength(), ITERATIONS + 500);
}

TEST_F(ArrayStressTest, EnumeratorUnderHighLoad) {
    MutableArraySequence<int> seq;
    const int ITERATIONS = 500000;
    for (int i = 0; i < ITERATIONS; ++i) {
        seq.Append(i);
    }
    
    IEnumerator<int>* en = seq.GetEnumerator();
    int readCount = 0;
    long long checksum = 0;
    
    ASSERT_NO_THROW({
        while (en->MoveNext()) {
            checksum += en->GetCurrent();
            readCount++;
        }
    });
    
    EXPECT_EQ(readCount, ITERATIONS);
    // Проверка суммы арифметической прогрессии для подтверждения целостности данных
    long long expectedChecksum = (1LL * (ITERATIONS - 1) * ITERATIONS) / 2;
    EXPECT_EQ(checksum, expectedChecksum);
    
    delete en;
}
