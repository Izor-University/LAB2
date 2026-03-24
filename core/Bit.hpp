#ifndef BIT_HPP
#define BIT_HPP

// Абстракция, позволяющая представить битовое значение
class Bit {
private:
    bool bitValue;

public:
    // Конструкторы
    Bit(bool initialValue = false);
    Bit(int initialValue);

    // Получение значения
    bool GetValue() const;

    // Перегрузка побитовых операторов
    Bit operator&(const Bit& other) const;
    Bit operator|(const Bit& other) const;
    Bit operator^(const Bit& other) const;
    Bit operator~() const;

    // Перегрузка операторов сравнения
    bool operator==(const Bit& other) const;
    bool operator!=(const Bit& other) const;
};

#endif // BIT_HPP