#ifndef BIT_HPP
#define BIT_HPP

#include <ostream>

// --- Абстракция битового значения ---
class Bit {
private:
    bool bitValue;

public:
    // --- Конструкторы ---
    Bit(bool initialValue = false);
    Bit(int initialValue);

    // --- Декомпозиция ---
    bool GetValue() const;

    // --- Перегрузка побитовых операторов ---
    Bit operator&(const Bit& other) const;
    Bit operator|(const Bit& other) const;
    Bit operator^(const Bit& other) const;
    Bit operator~() const;

    // --- Перегрузка операторов сравнения ---
    bool operator==(const Bit& other) const;
    bool operator!=(const Bit& other) const;
};

// --- Оператор вывода ---
std::ostream& operator<<(std::ostream& os, const Bit& bit);

#endif // BIT_HPP