#ifndef BIT_SEQUENCE_HPP
#define BIT_SEQUENCE_HPP

#include "MutableArraySequence.hpp"
#include "Bit.hpp"

// --- Битовая последовательность ---
class BitSequence : public MutableArraySequence<Bit> {
public:
    // --- Конструкторы ---
    BitSequence();
    BitSequence(Bit* items, int count);
    BitSequence(const Sequence<Bit>& sequence);

    // --- Побитовые операции ---
    BitSequence* And(const BitSequence* other) const;
    BitSequence* Or(const BitSequence* other) const;
    BitSequence* Xor(const BitSequence* other) const;
    BitSequence* Not() const;
};

#endif // BIT_SEQUENCE_HPP