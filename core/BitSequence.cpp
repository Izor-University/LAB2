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
    int maxLength = this->GetLength();
    if (other->GetLength() > maxLength) {
        maxLength = other->GetLength();
    }

    BitSequence* resultSequence = new BitSequence();

    for (int i = 0; i < maxLength; ++i) {
        Bit bit1 = Bit(false);
        if (i < this->GetLength()) {
            bit1 = this->Get(i);
        }

        Bit bit2 = Bit(false);
        if (i < other->GetLength()) {
            bit2 = other->Get(i);
        }

        resultSequence->AppendInternal(bit1 & bit2);
    }

    return resultSequence;
}

// Побитовое ИЛИ (OR)
BitSequence* BitSequence::Or(const BitSequence* other) const {
    int maxLength = this->GetLength();
    if (other->GetLength() > maxLength) {
        maxLength = other->GetLength();
    }

    BitSequence* resultSequence = new BitSequence();

    for (int i = 0; i < maxLength; ++i) {
        Bit bit1 = Bit(false);
        if (i < this->GetLength()) {
            bit1 = this->Get(i);
        }

        Bit bit2 = Bit(false);
        if (i < other->GetLength()) {
            bit2 = other->Get(i);
        }

        resultSequence->AppendInternal(bit1 | bit2);
    }

    return resultSequence;
}

// Побитовое исключающее ИЛИ (XOR)
BitSequence* BitSequence::Xor(const BitSequence* other) const {
    int maxLength = this->GetLength();
    if (other->GetLength() > maxLength) {
        maxLength = other->GetLength();
    }

    BitSequence* resultSequence = new BitSequence();

    for (int i = 0; i < maxLength; ++i) {
        Bit bit1 = Bit(false);
        if (i < this->GetLength()) {
            bit1 = this->Get(i);
        }

        Bit bit2 = Bit(false);
        if (i < other->GetLength()) {
            bit2 = other->Get(i);
        }

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