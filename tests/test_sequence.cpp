#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include "Sequence.hpp"
#include "MutableArraySequence.hpp"
#include "Exceptions.hpp"
#include "Option.hpp"
#include "SequenceIO.hpp"

// ============================================================================
// ИНСТРУМЕНТЫ ДЛЯ СТРОГОГО АНАЛИЗА ПАМЯТИ
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

class SequenceBaseMemoryTest : public ::testing::Test {
protected:
    void SetUp() override { InstanceTracker::activeInstances = 0; }
    void TearDown() override { EXPECT_EQ(InstanceTracker::activeInstances, 0) << "BASE CLASS LEAKED MEMORY!"; }
};

// Специальное исключение для имитации сбоев внутри пользовательских функций Map/Where
class TestException : public std::runtime_error {
public:
    TestException() : std::runtime_error("Simulated failure") {}
};

InstanceTracker ThrowOnValue3(const InstanceTracker& x) {
    if (x.value == 3) throw TestException();
    return InstanceTracker(x.value * 2);
}

bool ThrowOnValue2(const InstanceTracker& x) {
    if (x.value == 2) throw TestException();
    return x.value % 2 == 0;
}

InstanceTracker ThrowOnReduceValue3(const InstanceTracker& acc, const InstanceTracker& cur) {
    if (cur.value == 3) throw TestException();
    return InstanceTracker(acc.value + cur.value);
}

// ============================================================================
// ПОЛИМОРФИЗМ И ВИРТУАЛЬНЫЙ ДЕСТРУКТОР
// ============================================================================

TEST_F(SequenceBaseMemoryTest, VirtualDestructorCleansUpDerived) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2)};
    
    // Upcasting: указатель базового класса смотрит на объект наследника
    Sequence<InstanceTracker>* basePtr = new MutableArraySequence<InstanceTracker>(arr, 2);
    
    EXPECT_EQ(basePtr->GetLength(), 2);
    EXPECT_EQ(InstanceTracker::activeInstances, 4); // 2 в arr, 2 в коллекции
    
    // Удаление через базовый указатель. Если деструктор Sequence не virtual, будет утечка!
    delete basePtr; 
    
    EXPECT_EQ(InstanceTracker::activeInstances, 2); // Остались только оригиналы в arr
}

// ============================================================================
// ДЕКОМПОЗИЦИЯ И ПОГРАНИЧНЫЕ СЛУЧАИ (GetFirst, GetLast)
// ============================================================================
class SequenceDecompositionTest : public ::testing::Test {};

TEST_F(SequenceDecompositionTest, BaseGettersFallbacks) {
    int arr[] = {10, 20, 30};
    Sequence<int>* seq = new MutableArraySequence<int>(arr, 3);
    
    EXPECT_EQ(seq->GetFirst(), 10);
    EXPECT_EQ(seq->GetLast(), 30);
    
    delete seq;
}

TEST_F(SequenceDecompositionTest, BaseGettersThrowOnEmpty) {
    Sequence<int>* emptySeq = new MutableArraySequence<int>();
    
    EXPECT_THROW(emptySeq->GetFirst(), EmptyCollectionError);
    EXPECT_THROW(emptySeq->GetLast(), EmptyCollectionError);
    
    delete emptySeq;
}

// ============================================================================
// ТЕСТИРОВАНИЕ БЕЗОПАСНОСТИ ИСКЛЮЧЕНИЙ (EXCEPTION SAFETY IN ALGORITHMS)
// ============================================================================
// Эти тесты доказывают, что блоки try-catch в Sequence.tpp очищают итераторы и билдеры.

TEST_F(SequenceBaseMemoryTest, MapExceptionSafety) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2), InstanceTracker(3), InstanceTracker(4)};
    Sequence<InstanceTracker>* seq = new MutableArraySequence<InstanceTracker>(arr, 4);
    
    // Передаем функцию, которая выбросит исключение на 3-м элементе.
    // Если try-catch в Sequence::Map написан неправильно, итератор или наполовину 
    // построенная коллекция навсегда останутся в памяти.
    EXPECT_THROW({
        seq->Map(ThrowOnValue3);
    }, TestException);
    
    delete seq;
    // TearDown проверит activeInstances == 0.
}

TEST_F(SequenceBaseMemoryTest, WhereExceptionSafety) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2), InstanceTracker(3)};
    Sequence<InstanceTracker>* seq = new MutableArraySequence<InstanceTracker>(arr, 3);
    
    EXPECT_THROW({
        seq->Where(ThrowOnValue2);
    }, TestException);
    
    delete seq;
}

TEST_F(SequenceBaseMemoryTest, ReduceExceptionSafety) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2), InstanceTracker(3)};
    Sequence<InstanceTracker>* seq = new MutableArraySequence<InstanceTracker>(arr, 3);
    
    EXPECT_THROW({
        seq->Reduce(ThrowOnReduceValue3, InstanceTracker(0));
    }, TestException);
    
    delete seq;
}

// ============================================================================
// АЛГОРИТМ SLICE (СРЕЗ С ВСТАВКОЙ)
// ============================================================================
class SequenceSliceTest : public ::testing::Test {};

TEST_F(SequenceSliceTest, SliceWithoutInsertion) {
    int arr[] = {0, 1, 2, 3, 4, 5};
    Sequence<int>* seq = new MutableArraySequence<int>(arr, 6);
    
    // Вырезаем 3 элемента, начиная с индекса 2 (элементы 2, 3, 4 удаляются)
    Sequence<int>* sliced = seq->Slice(2, 3, nullptr);
    
    EXPECT_EQ(sliced->GetLength(), 3);
    EXPECT_EQ(sliced->Get(0), 0);
    EXPECT_EQ(sliced->Get(1), 1);
    EXPECT_EQ(sliced->Get(2), 5); // Элементы "схлопнулись"
    
    delete sliced;
    delete seq;
}

TEST_F(SequenceSliceTest, SliceWithInsertion) {
    int arr1[] = {0, 1, 2, 3};
    int arr2[] = {99, 88};
    Sequence<int>* seq = new MutableArraySequence<int>(arr1, 4);
    Sequence<int>* insertSeq = new MutableArraySequence<int>(arr2, 2);
    
    // Заменяем 1 элемент (индекс 1) на insertSeq
    Sequence<int>* sliced = seq->Slice(1, 1, insertSeq);
    
    EXPECT_EQ(sliced->GetLength(), 5);
    EXPECT_EQ(sliced->Get(0), 0);
    EXPECT_EQ(sliced->Get(1), 99); // Начало вставки
    EXPECT_EQ(sliced->Get(2), 88); // Конец вставки
    EXPECT_EQ(sliced->Get(3), 2);
    EXPECT_EQ(sliced->Get(4), 3);
    
    delete sliced;
    delete insertSeq;
    delete seq;
}

TEST_F(SequenceSliceTest, SliceNegativeIndicesAndBounds) {
    int arr[] = {10, 20, 30};
    Sequence<int>* seq = new MutableArraySequence<int>(arr, 3);
    
    // Отрицательный индекс должен отсчитываться с конца: -1 -> индекс 2
    Sequence<int>* sliced = seq->Slice(-1, 1, nullptr);
    EXPECT_EQ(sliced->GetLength(), 2);
    EXPECT_EQ(sliced->GetLast(), 20); // 30 было вырезано
    
    // count выходит за пределы (должен обрезаться до конца массива)
    Sequence<int>* slicedOver = seq->Slice(1, 100, nullptr);
    EXPECT_EQ(slicedOver->GetLength(), 1);
    EXPECT_EQ(slicedOver->GetFirst(), 10);
    
    // Явно некорректные границы
    EXPECT_THROW(seq->Slice(5, 1, nullptr), IndexOutOfRange);
    EXPECT_THROW(seq->Slice(-5, 1, nullptr), IndexOutOfRange);
    
    delete sliced;
    delete slicedOver;
    delete seq;
}

// ============================================================================
// TRY-СЕМАНТИКА (РАБОТА С OPTION)
// ============================================================================
class SequenceOptionTest : public ::testing::Test {};

TEST_F(SequenceOptionTest, TryGetMethodsReturnsValidOption) {
    int arr[] = {42};
    Sequence<int>* seq = new MutableArraySequence<int>(arr, 1);
    
    Option<int> optGet = seq->TryGet(0);
    Option<int> optFirst = seq->TryGetFirst();
    Option<int> optLast = seq->TryGetLast();
    
    EXPECT_TRUE(optGet.IsSome());
    EXPECT_EQ(optGet.GetValue(), 42);
    
    EXPECT_TRUE(optFirst.IsSome());
    EXPECT_EQ(optFirst.GetValue(), 42);
    
    EXPECT_TRUE(optLast.IsSome());
    EXPECT_EQ(optLast.GetValue(), 42);
    
    delete seq;
}

TEST_F(SequenceOptionTest, TryGetMethodsReturnsEmptyOptionOnFailure) {
    Sequence<int>* emptySeq = new MutableArraySequence<int>();
    
    Option<int> optGet = emptySeq->TryGet(0);
    Option<int> optGetOutOfBounds = emptySeq->TryGet(5);
    Option<int> optFirst = emptySeq->TryGetFirst();
    Option<int> optLast = emptySeq->TryGetLast();
    
    // Проверяем, что значений нет (можно использовать IsNone или EXPECT_FALSE(IsSome))
    EXPECT_TRUE(optGet.IsNone());
    EXPECT_TRUE(optGetOutOfBounds.IsNone());
    EXPECT_TRUE(optFirst.IsNone());
    EXPECT_TRUE(optLast.IsNone());
    
    delete emptySeq;
}

// ============================================================================
// ПОТОКОВЫЙ ВЫВОД (OPERATOR<<)
// ============================================================================
TEST_F(SequenceDecompositionTest, StreamOperatorFormatting) {
    int arr[] = {1, 2, 3};
    Sequence<int>* seq = new MutableArraySequence<int>(arr, 3);
    
    std::stringstream ss;
    ss << *seq;
    
    EXPECT_EQ(ss.str(), "[1, 2, 3]");
    
    Sequence<int>* emptySeq = new MutableArraySequence<int>();
    std::stringstream ssEmpty;
    ssEmpty << *emptySeq;
    
    EXPECT_EQ(ssEmpty.str(), "[]");
    
    delete emptySeq;
    delete seq;
}
