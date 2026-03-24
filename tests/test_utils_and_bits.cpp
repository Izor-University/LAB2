#include <gtest/gtest.h>
#include "../core/Utils.hpp"
#include "../core/BitSequence.hpp"

// --- 1. Основные участки предметной логики ---
TEST(BitSequenceTest, SubjectLogic_BitOperations) {
    BitSequence seq1;
    seq1.Append(Bit(1)); seq1.Append(Bit(0)); // [1, 0]

    BitSequence seq2;
    seq2.Append(Bit(1)); seq2.Append(Bit(1)); // [1, 1]

    BitSequence* andRes = seq1.And(&seq2);
    EXPECT_EQ(andRes->Get(0).GetValue(), true);
    EXPECT_EQ(andRes->Get(1).GetValue(), false);

    BitSequence* orRes = seq1.Or(&seq2);
    EXPECT_EQ(orRes->Get(0).GetValue(), true);
    EXPECT_EQ(orRes->Get(1).GetValue(), true);

    BitSequence* xorRes = seq1.Xor(&seq2);
    EXPECT_EQ(xorRes->Get(0).GetValue(), false);
    EXPECT_EQ(xorRes->Get(1).GetValue(), true);

    BitSequence* notRes = seq1.Not();
    EXPECT_EQ(notRes->Get(0).GetValue(), false);
    EXPECT_EQ(notRes->Get(1).GetValue(), true);

    delete andRes; delete orRes; delete xorRes; delete notRes;
}

TEST(UtilsTest, SubjectLogic_SplitSequence) {
    MutableArraySequence<int> seq;
    seq.Append(1); seq.Append(0); seq.Append(2); seq.Append(3);

    Sequence<Sequence<int>*>* chunks = SequenceUtils::Split<int>(seq, 0);

    EXPECT_EQ(chunks->GetLength(), 2);
    EXPECT_EQ(chunks->Get(0)->GetLength(), 1); // [1]
    EXPECT_EQ(chunks->Get(1)->GetLength(), 2); // [2, 3]

    for (int i = 0; i < chunks->GetLength(); ++i) delete chunks->Get(i);
    delete chunks;
}

TEST(UtilsTest, SubjectLogic_ZipAndUnzip) {
    MutableArraySequence<int> ints;
    ints.Append(1); ints.Append(2);

    MutableArraySequence<char> chars;
    chars.Append('A'); chars.Append('B');

    // Проверка Zip
    Sequence<Pair<int, char>>* zipped = SequenceUtils::Zip<int, char>(ints, chars);
    EXPECT_EQ(zipped->GetLength(), 2);
    EXPECT_EQ(zipped->Get(0).first, 1);
    EXPECT_EQ(zipped->Get(0).second, 'A');

    // Проверка Unzip
    Pair<Sequence<int>*, Sequence<char>*> unzipped = SequenceUtils::Unzip<int, char>(*zipped);
    EXPECT_EQ(unzipped.first->GetLength(), 2);
    EXPECT_EQ(unzipped.second->Get(1), 'B');

    delete zipped;
    delete unzipped.first;
    delete unzipped.second;
}

// --- 2. Охват пограничных ситуаций ---
TEST(UtilsTest, EdgeCases_ZipDifferentLengths) {
    MutableArraySequence<int> ints;
    ints.Append(1); ints.Append(2); ints.Append(3); // Длина 3
    
    MutableArraySequence<char> chars;
    chars.Append('A'); chars.Append('B'); // Длина 2
    
    // Zip должен безопасно отработать по минимальной длине (2)
    Sequence<Pair<int, char>>* zipped = SequenceUtils::Zip<int, char>(ints, chars);
    EXPECT_EQ(zipped->GetLength(), 2);
    
    delete zipped;
}

TEST(BitSequenceTest, EdgeCases_OperateOnDifferentLengths) {
    BitSequence seq1; // Длина 1
    seq1.Append(Bit(1));
    
    BitSequence seq2; // Длина 2
    seq2.Append(Bit(1)); seq2.Append(Bit(1));
    
    // AND должен обработать недостающий бит seq1 как 0 (false)
    BitSequence* andRes = seq1.And(&seq2);
    EXPECT_EQ(andRes->GetLength(), 2);
    EXPECT_EQ(andRes->Get(1).GetValue(), false);
    
    delete andRes;
}