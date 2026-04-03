#include <gtest/gtest.h>
#include <utility>
#include "MutableListSequence.hpp"
#include "ImmutableListSequence.hpp"
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

class ListSequenceMemoryTest : public ::testing::Test {
protected:
    void SetUp() override { InstanceTracker::activeInstances = 0; }
    void TearDown() override { EXPECT_EQ(InstanceTracker::activeInstances, 0) << "MEMORY LEAK DETECTED IN SEQUENCE ABSTRACTION!"; }
};

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ПРЕДИКАТЫ
// ============================================================================
InstanceTracker MultiplyTracker(const InstanceTracker& x) { return InstanceTracker(x.value * 2); }
bool IsTrackerEven(const InstanceTracker& x) { return x.value % 2 == 0; }
InstanceTracker SumTrackers(const InstanceTracker& acc, const InstanceTracker& cur) { 
    return InstanceTracker(acc.value + cur.value); 
}

// ============================================================================
// MUTABLE LIST SEQUENCE: БАЗОВЫЙ КОНТРАКТ И МУТАЦИИ
// ============================================================================
class MutableListSequenceTest : public ::testing::Test {};

TEST_F(MutableListSequenceTest, EmptySequenceSemantics) {
    MutableListSequence<int> seq;
    EXPECT_EQ(seq.GetLength(), 0);
    EXPECT_THROW(seq.GetFirst(), EmptyCollectionError);
    EXPECT_THROW(seq.GetLast(), EmptyCollectionError);
    EXPECT_THROW(seq.Get(0), IndexOutOfRange);
}

TEST_F(ListSequenceMemoryTest, MutableInPlaceModifications) {
    MutableListSequence<InstanceTracker> seq;
    
    // Append возвращает указатель на тот же объект (this)
    Sequence<InstanceTracker>* res1 = seq.Append(InstanceTracker(10));
    EXPECT_EQ(res1, &seq);
    EXPECT_EQ(seq.GetLength(), 1);
    
    Sequence<InstanceTracker>* res2 = seq.Prepend(InstanceTracker(5));
    EXPECT_EQ(res2, &seq);
    EXPECT_EQ(seq.GetFirst().value, 5);
    
    Sequence<InstanceTracker>* res3 = seq.InsertAt(InstanceTracker(7), 1);
    EXPECT_EQ(res3, &seq);
    EXPECT_EQ(seq.Get(1).value, 7);
    EXPECT_EQ(seq.GetLength(), 3);
}

TEST_F(ListSequenceMemoryTest, MutableDeepCopyAndIndependence) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2)};
    MutableListSequence<InstanceTracker> original(arr, 2);
    
    MutableListSequence<InstanceTracker> copy(original);
    
    EXPECT_EQ(copy.GetLength(), 2);
    
    // Мутируем копию, оригинал должен остаться нетронутым
    copy.Append(InstanceTracker(3));
    EXPECT_EQ(copy.GetLength(), 3);
    EXPECT_EQ(original.GetLength(), 2);
}

TEST_F(ListSequenceMemoryTest, StrongExceptionGuaranteeOnModifiers) {
    InstanceTracker arr[] = {InstanceTracker(10), InstanceTracker(20)};
    MutableListSequence<InstanceTracker> seq(arr, 2);
    
    int originalLength = seq.GetLength();
    
    // Вставка с некорректным индексом
    EXPECT_THROW(seq.InsertAt(InstanceTracker(99), -1), IndexOutOfRange);
    EXPECT_THROW(seq.InsertAt(InstanceTracker(99), 5), IndexOutOfRange);
    
    // После ошибки объект обязан сохранить консистентность
    EXPECT_EQ(seq.GetLength(), originalLength);
    EXPECT_EQ(seq.GetFirst().value, 10);
    EXPECT_EQ(seq.GetLast().value, 20);
}

// ============================================================================
// IMMUTABLE LIST SEQUENCE: АБСОЛЮТНАЯ ИЗОЛЯЦИЯ ДАННЫХ
// ============================================================================
class ImmutableListSequenceTest : public ::testing::Test {};

TEST_F(ListSequenceMemoryTest, StrictImmutabilityContract) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2)};
    ImmutableListSequence<InstanceTracker> original(arr, 2);
    
    Sequence<InstanceTracker>* modified1 = original.Append(InstanceTracker(3));
    Sequence<InstanceTracker>* modified2 = original.Prepend(InstanceTracker(0));
    Sequence<InstanceTracker>* modified3 = original.InsertAt(InstanceTracker(99), 1);
    
    // 1. Указатели должны строго отличаться (созданы новые объекты)
    EXPECT_NE(modified1, &original);
    EXPECT_NE(modified2, &original);
    EXPECT_NE(modified3, &original);
    
    // 2. Оригинальная последовательность осталась неприкосновенной
    EXPECT_EQ(original.GetLength(), 2);
    EXPECT_EQ(original.GetFirst().value, 1);
    EXPECT_EQ(original.GetLast().value, 2);
    
    // 3. Новые последовательности содержат правильные данные
    EXPECT_EQ(modified1->GetLength(), 3);
    EXPECT_EQ(modified1->GetLast().value, 3);
    
    EXPECT_EQ(modified2->GetLength(), 3);
    EXPECT_EQ(modified2->GetFirst().value, 0);
    
    EXPECT_EQ(modified3->GetLength(), 3);
    EXPECT_EQ(modified3->Get(1).value, 99);
    
    // Освобождение памяти, так как Immutable методы вернули новые объекты (куча)
    delete modified1;
    delete modified2;
    delete modified3;
}

// ============================================================================
// АЛГОРИТМЫ ЯДРА (MAP, WHERE, REDUCE) ЧЕРЕЗ БЫСТРЫЙ ИТЕРАТОР
// ============================================================================
TEST_F(ListSequenceMemoryTest, FunctionalMapWithMemorySafety) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2), InstanceTracker(3)};
    MutableListSequence<InstanceTracker> seq(arr, 3);
    
    // Map должен использовать Builder и Итератор, возвращая новый Sequence
    Sequence<InstanceTracker>* mapped = seq.Map(MultiplyTracker);
    
    EXPECT_NE(mapped, &seq);
    EXPECT_EQ(mapped->GetLength(), 3);
    EXPECT_EQ(mapped->Get(0).value, 2);
    EXPECT_EQ(mapped->Get(1).value, 4);
    EXPECT_EQ(mapped->Get(2).value, 6);
    
    delete mapped;
}

TEST_F(ListSequenceMemoryTest, FunctionalWhereFiltering) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2), InstanceTracker(3), InstanceTracker(4)};
    ImmutableListSequence<InstanceTracker> seq(arr, 4);
    
    Sequence<InstanceTracker>* filtered = seq.Where(IsTrackerEven);
    
    EXPECT_EQ(filtered->GetLength(), 2);
    EXPECT_EQ(filtered->Get(0).value, 2);
    EXPECT_EQ(filtered->Get(1).value, 4);
    
    delete filtered;
}

TEST_F(ListSequenceMemoryTest, ReduceAggregation) {
    InstanceTracker arr[] = {InstanceTracker(10), InstanceTracker(20), InstanceTracker(30)};
    MutableListSequence<InstanceTracker> seq(arr, 3);
    
    InstanceTracker startVal(0);
    InstanceTracker result = seq.Reduce(SumTrackers, startVal);
    
    EXPECT_EQ(result.value, 60);
}

// ============================================================================
// ДЕКОМПОЗИЦИЯ: CONCAT И GETSUBSEQUENCE
// ============================================================================
TEST_F(ListSequenceMemoryTest, ConcatPolymorphism) {
    InstanceTracker arr1[] = {InstanceTracker(1)};
    InstanceTracker arr2[] = {InstanceTracker(2)};
    MutableListSequence<InstanceTracker> seq1(arr1, 1);
    ImmutableListSequence<InstanceTracker> seq2(arr2, 1);
    
    // Concat должен уметь склеивать Mutable и Immutable через интерфейс Sequence
    Sequence<InstanceTracker>* combined = seq1.Concat(&seq2);
    
    EXPECT_EQ(combined->GetLength(), 2);
    EXPECT_EQ(combined->GetFirst().value, 1);
    EXPECT_EQ(combined->GetLast().value, 2);
    
    delete combined;
}

TEST_F(ListSequenceMemoryTest, SubsequenceExtraction) {
    InstanceTracker arr[] = {InstanceTracker(0), InstanceTracker(10), InstanceTracker(20), InstanceTracker(30)};
    MutableListSequence<InstanceTracker> seq(arr, 4);
    
    Sequence<InstanceTracker>* sub = seq.GetSubsequence(1, 2);
    
    EXPECT_EQ(sub->GetLength(), 2);
    EXPECT_EQ(sub->GetFirst().value, 10);
    EXPECT_EQ(sub->GetLast().value, 20);
    
    EXPECT_THROW(seq.GetSubsequence(-1, 2), IndexOutOfRange);
    EXPECT_THROW(seq.GetSubsequence(2, 1), IndexOutOfRange);
    
    delete sub;
}

// ============================================================================
// ИНТЕГРАЦИЯ С ENUMERATOR НА УРОВНЕ SEQUENCE
// ============================================================================
class ListSequenceEnumeratorTest : public ::testing::Test {};

TEST_F(ListSequenceEnumeratorTest, SequenceProvidesWorkingEnumerator) {
    int arr[] = {77, 88};
    MutableListSequence<int> seq(arr, 2);
    
    IEnumerator<int>* en = seq.GetEnumerator(); // Получаем итератор от обертки
    
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 77);
    
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 88);
    
    EXPECT_FALSE(en->MoveNext());
    
    delete en;
}

// ============================================================================
// СТРЕСС-ТЕСТЫ: ПРОВЕРКА O(N) И БЕЗОПАСНОСТИ АБСТРАКЦИЙ
// ============================================================================
class ListSequenceStressTest : public ::testing::Test {};

TEST_F(ListSequenceStressTest, ExtremeScaleMapExecution) {
    MutableListSequence<int> seq;
    const int ELEMENTS = 100000;
    
    // Если Append() использует O(1), это сработает мгновенно
    for (int i = 0; i < ELEMENTS; ++i) {
        seq.Append(i);
    }
    
    Sequence<int>* mapped = nullptr;
    
    // Если `Map` использует старый `Get(i)` внутри `LinkedList`,
    // мы получим O(N^2) (10 миллиардов операций) и тест зависнет.
    // Если используется новый быстрый `Enumerator`, тест пролетит за O(N).
    ASSERT_NO_THROW({
        mapped = seq.Map([](const int& val) { return val + 1; });
    });
    
    EXPECT_EQ(mapped->GetLength(), ELEMENTS);
    EXPECT_EQ(mapped->GetFirst(), 1);
    EXPECT_EQ(mapped->GetLast(), ELEMENTS);
    
    delete mapped;
}

TEST_F(ListSequenceStressTest, PolymorphicConcatAtScale) {
    MutableListSequence<int> seq1;
    ImmutableListSequence<int> seq2;
    
    const int ELEMENTS = 50000;
    for (int i = 0; i < ELEMENTS; ++i) {
        seq1.Append(1);
        // seq2.Append вернул бы новый объект, поэтому мы не заполняем Immutable
        // в цикле напрямую, чтобы не плодить 50к объектов. Мы тестируем саму склейку.
    }
    
    Sequence<int>* combined = nullptr;
    
    ASSERT_NO_THROW({
        combined = seq1.Concat(&seq1); // Склеиваем саму с собой для масштаба
    });
    
    EXPECT_EQ(combined->GetLength(), ELEMENTS * 2);
    
    delete combined;
}
