#ifndef BIT_SEQUENCE_HPP
#define BIT_SEQUENCE_HPP

#include "MutableArraySequence.hpp"
#include "Bit.hpp" // Подключаем наш новый файл

// Битовая последовательность
class BitSequence : public MutableArraySequence<Bit> {
public:
    // Конструкторы
    BitSequence();
    BitSequence(Bit* items, int count);

    // Конструктор копирования из любой другой Sequence<Bit>
    BitSequence(const Sequence<Bit>& sequence);

    // Специфичные операции для битовых массивов
    // Возвращают новую битовую последовательность (маску)
    BitSequence* And(const BitSequence* other) const;
    BitSequence* Or(const BitSequence* other) const;
    BitSequence* Xor(const BitSequence* other) const;
    BitSequence* Not() const;
};

#endif // BIT_SEQUENCE_HPP