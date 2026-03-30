#include "Bit.hpp"

// --- Конструкторы ---
Bit::Bit(bool initialValue) {
    bitValue = initialValue;
}

Bit::Bit(int initialValue) {
    if (initialValue != 0) {
        bitValue = true;
    } else {
        bitValue = false;
    }
}

// --- Декомпозиция ---
bool Bit::GetValue() const {
    return bitValue;
}

// --- Перегрузка побитовых операторов ---
Bit Bit::operator&(const Bit& other) const {
    bool result = bitValue && other.bitValue;
    return Bit(result);
}

Bit Bit::operator|(const Bit& other) const {
    bool result = bitValue || other.bitValue;
    return Bit(result);
}

Bit Bit::operator^(const Bit& other) const {
    bool result = bitValue != other.bitValue;
    return Bit(result);
}

Bit Bit::operator~() const {
    bool result = !bitValue;
    return Bit(result);
}

// --- Перегрузка операторов сравнения ---
bool Bit::operator==(const Bit& other) const {
    return bitValue == other.bitValue;
}

bool Bit::operator!=(const Bit& other) const {
    return bitValue != other.bitValue;
}

// --- Оператор вывода ---
std::ostream& operator<<(std::ostream& os, const Bit& bit) {
    os << (bit.GetValue() ? 1 : 0);
    return os;
}