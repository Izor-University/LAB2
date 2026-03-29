#include "BitSequence.hpp"

// Вызов конструкторов базового класса
BitSequence::BitSequence() : MutableArraySequence<Bit>() {
}

BitSequence::BitSequence(Bit* items, int count) : MutableArraySequence<Bit>(items, count) {
}

BitSequence::BitSequence(const Sequence<Bit>& sequence) : MutableArraySequence<Bit>() {
    for (int i = 0; i < sequence.GetLength(); ++i) {
        this->AppendInternal(sequence.Get(i));
    }
}

// Побитовое И (AND)
BitSequence* BitSequence::And(const BitSequence* other) const {
    int len1 = this->GetLength();
    int len2 = other->GetLength();
    int maxLength = (len1 > len2) ? len1 : len2;

    // Смещение для выравнивания по правому краю (добавление нулей в начало)
    int offsetThis = maxLength - len1;
    int offsetOther = maxLength - len2;

    BitSequence* resultSequence = new BitSequence();

    for (int i = 0; i < maxLength; ++i) {
        Bit bit1 = (i >= offsetThis) ? this->Get(i - offsetThis) : Bit(false);
        Bit bit2 = (i >= offsetOther) ? other->Get(i - offsetOther) : Bit(false);

        resultSequence->AppendInternal(bit1 & bit2);
    }

    return resultSequence;
}

// Побитовое ИЛИ (OR)
BitSequence* BitSequence::Or(const BitSequence* other) const {
    int len1 = this->GetLength();
    int len2 = other->GetLength();
    int maxLength = (len1 > len2) ? len1 : len2;

    int offsetThis = maxLength - len1;
    int offsetOther = maxLength - len2;

    BitSequence* resultSequence = new BitSequence();

    for (int i = 0; i < maxLength; ++i) {
        Bit bit1 = (i >= offsetThis) ? this->Get(i - offsetThis) : Bit(false);
        Bit bit2 = (i >= offsetOther) ? other->Get(i - offsetOther) : Bit(false);

        resultSequence->AppendInternal(bit1 | bit2);
    }

    return resultSequence;
}

// Побитовое исключающее ИЛИ (XOR)
BitSequence* BitSequence::Xor(const BitSequence* other) const {
    int len1 = this->GetLength();
    int len2 = other->GetLength();
    int maxLength = (len1 > len2) ? len1 : len2;

    int offsetThis = maxLength - len1;
    int offsetOther = maxLength - len2;

    BitSequence* resultSequence = new BitSequence();

    for (int i = 0; i < maxLength; ++i) {
        Bit bit1 = (i >= offsetThis) ? this->Get(i - offsetThis) : Bit(false);
        Bit bit2 = (i >= offsetOther) ? other->Get(i - offsetOther) : Bit(false);

        resultSequence->AppendInternal(bit1 ^ bit2);
    }

    return resultSequence;
}

// Побитовое НЕ (NOT)
BitSequence* BitSequence::Not() const {
    BitSequence* resultSequence = new BitSequence();

    for (int i = 0; i < this->GetLength(); ++i) {
        resultSequence->AppendInternal(~this->Get(i));
    }

    return resultSequence;
}