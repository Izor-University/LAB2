#include <gtest/gtest.h>
#include "../core/MutableArraySequence.hpp"
#include "../core/MutableListSequence.hpp"
#include "../core/ImmutableListSequence.hpp"
#include "../core/SequenceBuilders.hpp"

namespace {
    int MultiplyByTwo(const int& x) { return x * 2; }
    bool IsEven(const int& x) { return x % 2 == 0; }
    int SumReduce(const int& a, const int& b) { return a + b; }
}

// --- 1. Основные участки предметной логики ---
TEST(SequenceTest, SubjectLogic_MapWhereReduce) {
    MutableArraySequence<int> seq;
    for (int i = 1; i <= 5; ++i) seq.Append(i);

    Sequence<int>* mapped = seq.map(MultiplyByTwo);
    EXPECT_EQ(mapped->Get(4), 10);

    Sequence<int>* evens = seq.where(IsEven);
    EXPECT_EQ(evens->GetLength(), 2);

    int sum = seq.reduce(SumReduce, 0);
    EXPECT_EQ(sum, 15);

    delete mapped; delete evens;
}

TEST(SequenceTest, SubjectLogic_PolymorphicOperations) {
    Sequence<int>* arraySeq = new MutableArraySequence<int>();
    Sequence<int>* listSeq = new MutableListSequence<int>();

    arraySeq->Append(1)->Prepend(0)->InsertAt(2, 2); // [0, 1, 2]
    listSeq->Append(1)->Prepend(0)->InsertAt(2, 2);  // [0, 1, 2]

    EXPECT_EQ(arraySeq->GetLast(), 2);
    EXPECT_EQ(listSeq->GetFirst(), 0);

    Sequence<int>* concated = arraySeq->Concat(listSeq);
    EXPECT_EQ(concated->GetLength(), 6);

    delete arraySeq; delete listSeq; delete concated;
}

TEST(SequenceTest, SubjectLogic_BuilderAndImmutable) {
    ImmutableListSequence<int> templateSeq;
    ISequenceBuilder<int>* builder = templateSeq.CreateBuilder();

    builder->Append(10)->Append(20);
    Sequence<int>* result = builder->Build();

    EXPECT_EQ(result->GetLength(), 2);
    EXPECT_NE(dynamic_cast<ImmutableListSequence<int>*>(result), nullptr);

    delete builder; delete result;
}

// --- 2. Охват пограничных ситуаций ---
TEST(SequenceTest, EdgeCases_SliceModifications) {
    MutableArraySequence<int> seq;
    seq.Append(1); seq.Append(2); seq.Append(3);

    // Срез с выходом за фактические границы длины (нормализация count)
    seq.Slice(1, 10, nullptr);
    EXPECT_EQ(seq.GetLength(), 1);
    EXPECT_EQ(seq.Get(0), 1);

    // Отрицательный индекс (отсчет с конца - нормализация index)
    seq.Append(2); seq.Append(3); // Теперь массив [1, 2, 3]
    seq.Slice(-1, 1, nullptr); // Должно удалить '3'
    EXPECT_EQ(seq.GetLast(), 2);
}

TEST(SequenceTest, EdgeCases_EmptySequenceOperations) {
    MutableListSequence<int> emptySeq;
    Sequence<int>* mapped = emptySeq.map(MultiplyByTwo);
    EXPECT_EQ(mapped->GetLength(), 0);
    delete mapped;
}

// --- 3. Некорректные ситуации и сценарии ---
TEST(SequenceTest, IncorrectScenarios_TrySemanticsProtection) {
    MutableArraySequence<int> emptySeq;

    // Вместо падения программы, мы должны получить корректный объект None
    Option<int> optFirst = emptySeq.TryGetFirst();
    EXPECT_TRUE(optFirst.IsNone());

    Option<int> optOut = emptySeq.TryGet(100);
    EXPECT_TRUE(optOut.IsNone());
}

TEST(SequenceTest, IncorrectScenarios_OptionUnwrap) {
    MutableArraySequence<int> emptySeq;
    Option<int> opt = emptySeq.TryGetFirst();

    // Попытка извлечь значение из None должна выбросить исключение
    EXPECT_THROW(opt.GetValue(), Exception);
}

TEST(SequenceTest, IncorrectScenarios_SequenceOutOfBounds) {
    MutableListSequence<int> seq;
    seq.Append(1);

    EXPECT_THROW(seq.Get(10), IndexOutOfRange);
    EXPECT_THROW(seq.InsertAt(5, 10), IndexOutOfRange);
}