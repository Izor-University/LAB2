#include <gtest/gtest.h>
#include "../core/BitSequence.hpp"
#include "../core/Exceptions.hpp"
#include "../core/IEnumerator.hpp"

// ============================================================================
// БАЗОВЫЕ ОПЕРАЦИИ И СОСТОЯНИЕ
// ============================================================================
class BitSequenceTest : public ::testing::Test {};

TEST_F(BitSequenceTest, DefaultConstructorState) {
    BitSequence seq;
    EXPECT_EQ(seq.GetLength(), 0);
    // Проверяем, что наша кастомная система исключений работает
    EXPECT_THROW(seq.GetBit(0), IndexOutOfRange);
}

TEST_F(BitSequenceTest, AppendAndGetLogic) {
    BitSequence seq;
    seq.AppendBit(true);  // 1
    seq.AppendBit(false); // 0
    seq.AppendBit(true);  // 1

    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_TRUE(seq.GetBit(0));
    EXPECT_FALSE(seq.GetBit(1));
    EXPECT_TRUE(seq.GetBit(2));
}

// ============================================================================
// УПРАВЛЕНИЕ ПАМЯТЬЮ (ГРАНИЦЫ БАЙТОВ)
// ============================================================================
class BitSequenceBoundaryTest : public ::testing::Test {};

TEST_F(BitSequenceBoundaryTest, ByteBoundaryCrossings) {
    BitSequence seq;

    // 1. Заполняем ровно 1 байт (8 бит)
    for (int i = 0; i < 8; ++i) {
        seq.AppendBit(i % 2 != 0); // 0, 1, 0, 1, 0, 1, 0, 1
    }

    EXPECT_EQ(seq.GetLength(), 8);
    EXPECT_TRUE(seq.GetBit(7));

    // 2. Добавление 9-го бита должно вызвать Resize() массива
    seq.AppendBit(true);
    EXPECT_EQ(seq.GetLength(), 9);
    EXPECT_TRUE(seq.GetBit(8));

    // 3. Заполняем до конца 4-го байта (32 бита)
    for (int i = 9; i < 32; ++i) {
        seq.AppendBit(false);
    }
    EXPECT_EQ(seq.GetLength(), 32);
    EXPECT_FALSE(seq.GetBit(31));

    // 4. Переход в 5-й байт (33-й бит)
    seq.AppendBit(true);
    EXPECT_EQ(seq.GetLength(), 33);
    EXPECT_TRUE(seq.GetBit(32));
}

// ============================================================================
// ПЕРЕГРУЗКА ПОБИТОВЫХ ОПЕРАТОРОВ
// ============================================================================
class BitSequenceOperatorsTest : public ::testing::Test {};

TEST_F(BitSequenceOperatorsTest, OperatorAnd) {
    BitSequence s1; // 1 0 1
    s1.AppendBit(true); s1.AppendBit(false); s1.AppendBit(true);

    BitSequence s2; // 1 1 0
    s2.AppendBit(true); s2.AppendBit(true); s2.AppendBit(false);

    BitSequence res = s1 & s2; // Ожидаем 1 0 0

    EXPECT_EQ(res.GetLength(), 3);
    EXPECT_TRUE(res.GetBit(0));
    EXPECT_FALSE(res.GetBit(1));
    EXPECT_FALSE(res.GetBit(2));
}

TEST_F(BitSequenceOperatorsTest, OperatorOr) {
    BitSequence s1; // 1 0 0
    s1.AppendBit(true); s1.AppendBit(false); s1.AppendBit(false);

    BitSequence s2; // 0 1 0
    s2.AppendBit(false); s2.AppendBit(true); s2.AppendBit(false);

    BitSequence res = s1 | s2; // Ожидаем 1 1 0

    EXPECT_EQ(res.GetLength(), 3);
    EXPECT_TRUE(res.GetBit(0));
    EXPECT_TRUE(res.GetBit(1));
    EXPECT_FALSE(res.GetBit(2));
}

TEST_F(BitSequenceOperatorsTest, OperatorXor) {
    BitSequence s1; // 1 1 0
    s1.AppendBit(true); s1.AppendBit(true); s1.AppendBit(false);

    BitSequence s2; // 1 0 1
    s2.AppendBit(true); s2.AppendBit(false); s2.AppendBit(true);

    BitSequence res = s1 ^ s2; // Ожидаем 0 1 1

    EXPECT_EQ(res.GetLength(), 3);
    EXPECT_FALSE(res.GetBit(0));
    EXPECT_TRUE(res.GetBit(1));
    EXPECT_TRUE(res.GetBit(2));
}

TEST_F(BitSequenceOperatorsTest, OperatorNot) {
    BitSequence s1; // 1 0 1
    s1.AppendBit(true); s1.AppendBit(false); s1.AppendBit(true);

    BitSequence res = ~s1; // Ожидаем 0 1 0

    EXPECT_EQ(res.GetLength(), 3);
    EXPECT_FALSE(res.GetBit(0));
    EXPECT_TRUE(res.GetBit(1));
    EXPECT_FALSE(res.GetBit(2));
}

// ============================================================================
// ПРАВИЛО ТРЕХ (КОПИРОВАНИЕ И ПРИСВАИВАНИЕ)
// ============================================================================
class BitSequenceRuleOfThreeTest : public ::testing::Test {};

TEST_F(BitSequenceRuleOfThreeTest, CopyConstructor) {
    BitSequence s1;
    s1.AppendBit(true);
    s1.AppendBit(false);

    BitSequence s2 = s1; // Вызов конструктора копирования

    EXPECT_EQ(s2.GetLength(), 2);
    EXPECT_TRUE(s2.GetBit(0));
    EXPECT_FALSE(s2.GetBit(1));

    // Проверка независимости памяти
    s1.AppendBit(true);
    EXPECT_EQ(s1.GetLength(), 3);
    EXPECT_EQ(s2.GetLength(), 2);
}

TEST_F(BitSequenceRuleOfThreeTest, AssignmentOperator) {
    BitSequence s1;
    s1.AppendBit(true);
    s1.AppendBit(false);

    BitSequence s2;
    s2.AppendBit(true); // Добавим мусор в s2

    s2 = s1; // Вызов оператора присваивания

    EXPECT_EQ(s2.GetLength(), 2);
    EXPECT_TRUE(s2.GetBit(0));
    EXPECT_FALSE(s2.GetBit(1));
}

TEST_F(BitSequenceRuleOfThreeTest, SelfAssignment) {
    BitSequence s1;
    s1.AppendBit(true);

    s1 = s1; // Самоприсваивание не должно сломать объект

    EXPECT_EQ(s1.GetLength(), 1);
    EXPECT_TRUE(s1.GetBit(0));
}

// ============================================================================
// ПРОВЕРКА ИТЕРАТОРА (ПОЛИМОРФИЗМ)
// ============================================================================
class BitSequenceEnumeratorTest : public ::testing::Test {};

TEST_F(BitSequenceEnumeratorTest, IterateThroughBits) {
    BitSequence seq;
    seq.AppendBit(false); // 0
    seq.AppendBit(true);  // 1
    seq.AppendBit(true);  // 1

    // Полиморфный вызов через интерфейс IEnumerable<bool>
    IEnumerator<bool>* en = seq.GetEnumerator();

    EXPECT_TRUE(en->MoveNext());
    EXPECT_FALSE(en->GetCurrent()); // 0

    EXPECT_TRUE(en->MoveNext());
    EXPECT_TRUE(en->GetCurrent());  // 1

    EXPECT_TRUE(en->MoveNext());
    EXPECT_TRUE(en->GetCurrent());  // 1

    EXPECT_FALSE(en->MoveNext());   // Конец

    // Проверка Reset
    en->Reset();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_FALSE(en->GetCurrent()); // Снова 0

    delete en; // Не забываем чистить память!
}