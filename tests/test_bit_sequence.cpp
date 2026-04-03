#include <gtest/gtest.h>
#include "BitSequence.hpp"
#include "Exceptions.hpp"
#include "IEnumerator.hpp"

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ И ПРЕДИКАТЫ ДЛЯ ТИПА BIT
// ============================================================================
Bit InvertBit(const Bit& bit) { return (bit == Bit(0)) ? Bit(1) : Bit(0); }
bool IsBitSet(const Bit& bit) { return bit == Bit(1); }
Bit LogicalOrBits(const Bit& acc, const Bit& cur) { 
    // Если хотя бы один из битов равен 1, возвращаем 1. Иначе 0.
    return (acc == Bit(1) || cur == Bit(1)) ? Bit(1) : Bit(0); 
}
// ============================================================================
// БАЗОВЫЕ ОПЕРАЦИИ И СОСТОЯНИЕ
// ============================================================================
class BitSequenceTest : public ::testing::Test {};

TEST_F(BitSequenceTest, DefaultConstructorState) {
    BitSequence seq;
    EXPECT_EQ(seq.GetLength(), 0);
    EXPECT_THROW(seq.GetFirst(), EmptyCollectionError);
    EXPECT_THROW(seq.GetLast(), EmptyCollectionError);
    EXPECT_THROW(seq.Get(0), IndexOutOfRange);
}

TEST_F(BitSequenceTest, InitializationFromArray) {
    Bit bits[] = {Bit(1), Bit(0), Bit(1), Bit(1), Bit(0)};
    BitSequence seq(bits, 5);
    
    EXPECT_EQ(seq.GetLength(), 5);
    EXPECT_EQ(seq.GetFirst(), Bit(1));
    EXPECT_EQ(seq.GetLast(), Bit(0));
    EXPECT_EQ(seq.Get(2), Bit(1));
    EXPECT_EQ(seq.Get(3), Bit(1));
}

// ============================================================================
// ПРОВЕРКА ГРАНИЦ БАЙТОВ И МАШИННЫХ СЛОВ
// ============================================================================
class BitSequenceBoundaryTest : public ::testing::Test {};

TEST_F(BitSequenceBoundaryTest, ByteBoundaryCrossings) {
    BitSequence seq;
    for (int i = 0; i < 8; ++i) {
        seq.Append(Bit(i % 2));
    }
    
    EXPECT_EQ(seq.GetLength(), 8);
    EXPECT_EQ(seq.Get(7), Bit(1));
    
    seq.Append(Bit(1));
    EXPECT_EQ(seq.GetLength(), 9);
    EXPECT_EQ(seq.Get(8), Bit(1));
    
    for (int i = 9; i < 32; ++i) {
        seq.Append(Bit(0));
    }
    EXPECT_EQ(seq.GetLength(), 32);
    EXPECT_EQ(seq.Get(31), Bit(0));
    
    seq.Append(Bit(1));
    EXPECT_EQ(seq.GetLength(), 33);
    EXPECT_EQ(seq.Get(32), Bit(1));
}

TEST_F(BitSequenceBoundaryTest, ShiftAcrossWordBoundaries) {
    BitSequence seq;
    for (int i = 0; i < 31; ++i) seq.Append(Bit(0));
    seq.Append(Bit(1)); 
    
    seq.Prepend(Bit(0));
    
    EXPECT_EQ(seq.GetLength(), 33);
    EXPECT_EQ(seq.Get(0), Bit(0));
    EXPECT_EQ(seq.Get(32), Bit(1)); 
}

// ============================================================================
// МУТАЦИИ И БЕЗОПАСНОСТЬ ПАМЯТИ
// ============================================================================
class BitSequenceMutationTest : public ::testing::Test {};

TEST_F(BitSequenceMutationTest, InsertAtInternalLogic) {
    Bit bits[] = {Bit(1), Bit(1), Bit(1)};
    BitSequence seq(bits, 3);
    
    Sequence<Bit>* res = seq.InsertAt(Bit(0), 1); 
    
    EXPECT_EQ(res, &seq);
    EXPECT_EQ(seq.GetLength(), 4);
    EXPECT_EQ(seq.Get(0), Bit(1));
    EXPECT_EQ(seq.Get(1), Bit(0));
    EXPECT_EQ(seq.Get(2), Bit(1));
    EXPECT_EQ(seq.Get(3), Bit(1));
}

TEST_F(BitSequenceMutationTest, ExceptionSafetyOnOutOfBounds) {
    BitSequence seq;
    seq.Append(Bit(1));
    
    int originalLength = seq.GetLength();
    
    EXPECT_THROW(seq.InsertAt(Bit(1), -1), IndexOutOfRange);
    EXPECT_THROW(seq.InsertAt(Bit(1), 2), IndexOutOfRange);
    EXPECT_THROW(seq.Get(1), IndexOutOfRange);
    EXPECT_THROW(seq.Get(-1), IndexOutOfRange);
    
    EXPECT_EQ(seq.GetLength(), originalLength);
}

// ============================================================================
// АЛГОРИТМЫ ЯДРА И РАБОТА С ENUMERATOR
// ============================================================================
class BitSequenceAlgorithmsTest : public ::testing::Test {};

TEST_F(BitSequenceAlgorithmsTest, MapInvertsBitsCorrectly) {
    Bit bits[] = {Bit(1), Bit(0), Bit(1), Bit(1), Bit(0)};
    BitSequence seq(bits, 5);
    
    Sequence<Bit>* inverted = seq.Map(InvertBit);
    
    EXPECT_NE(inverted, &seq);
    EXPECT_EQ(inverted->GetLength(), 5);
    EXPECT_EQ(inverted->Get(0), Bit(0));
    EXPECT_EQ(inverted->Get(1), Bit(1));
    EXPECT_EQ(inverted->Get(2), Bit(0));
    
    delete inverted;
}

TEST_F(BitSequenceAlgorithmsTest, WhereFiltersActiveBits) {
    Bit bits[] = {Bit(1), Bit(0), Bit(0), Bit(1), Bit(1), Bit(0)};
    BitSequence seq(bits, 6);
    
    Sequence<Bit>* onlyOnes = seq.Where(IsBitSet);
    
    EXPECT_EQ(onlyOnes->GetLength(), 3);
    EXPECT_EQ(onlyOnes->Get(0), Bit(1));
    EXPECT_EQ(onlyOnes->Get(1), Bit(1));
    EXPECT_EQ(onlyOnes->Get(2), Bit(1));
    
    delete onlyOnes;
}

TEST_F(BitSequenceAlgorithmsTest, ReduceCalculatesLogicalOr) {
    Bit bits[] = {Bit(0), Bit(0), Bit(1), Bit(0)};
    BitSequence seq(bits, 4);
    
    // Проверяем, есть ли хотя бы одна единица в последовательности
    Bit result = seq.Reduce(LogicalOrBits, Bit(0)); 
    EXPECT_EQ(result, Bit(1));

    // Проверяем полностью нулевую последовательность
    Bit zeroBits[] = {Bit(0), Bit(0), Bit(0)};
    BitSequence zeroSeq(zeroBits, 3);
    
    Bit zeroResult = zeroSeq.Reduce(LogicalOrBits, Bit(0));
    EXPECT_EQ(zeroResult, Bit(0));
}

TEST_F(BitSequenceAlgorithmsTest, ConcatUnalignedSequences) {
    Bit bits1[] = {Bit(1), Bit(0), Bit(1)}; 
    BitSequence seq1(bits1, 3);
    
    Bit bits2[] = {Bit(0), Bit(1), Bit(1), Bit(0), Bit(1)}; 
    BitSequence seq2(bits2, 5);
    
    Sequence<Bit>* result = seq1.Concat(&seq2);
    
    EXPECT_EQ(result->GetLength(), 8);
    EXPECT_EQ(result->Get(2), Bit(1)); 
    EXPECT_EQ(result->Get(3), Bit(0)); 
    EXPECT_EQ(result->Get(7), Bit(1)); 
    
    delete result;
}

TEST_F(BitSequenceAlgorithmsTest, SubsequenceExtractionAcrossBoundaries) {
    BitSequence seq;
    for (int i = 0; i < 16; ++i) {
        seq.Append(Bit(i % 2));
    }
    
    Sequence<Bit>* sub = seq.GetSubsequence(3, 10);
    
    EXPECT_EQ(sub->GetLength(), 8);
    EXPECT_EQ(sub->Get(0), Bit(1)); 
    EXPECT_EQ(sub->Get(7), Bit(0)); 
    
    delete sub;
}

// ============================================================================
// НАГРУЗОЧНЫЕ И СТРЕСС-ТЕСТЫ
// ============================================================================
class BitSequenceStressTest : public ::testing::Test {};

TEST_F(BitSequenceStressTest, EnumeratorOverMassiveSequence) {
    BitSequence seq;
    const int ITERATIONS = 500000;
    for (int i = 0; i < ITERATIONS; ++i) {
        seq.Append(Bit(1)); 
    }
    
    IEnumerator<Bit>* en = seq.GetEnumerator();
    int count = 0; // Для подсчета используем int
    
    ASSERT_NO_THROW({
        while (en->MoveNext()) {
            if (en->GetCurrent() == Bit(1)) {
                count++;
            }
        }
    });
    
    EXPECT_EQ(count, ITERATIONS); 
    
    delete en;
}
