#ifndef OPTION_HPP
#define OPTION_HPP

#include "exceptions.hpp"

template <class T>
class Option {
private:
    T* value; // Указатель на значение. Если nullptr — значит это None.

public:
    // Конструктор для состояния "None" (нет значения)
    Option() : value(nullptr) {}

    // Конструктор для состояния "Some" (есть значение)
    Option(const T& val) {
        value = new T(val);
    }

    // Правило трех: Копирующий конструктор, оператор присваивания, деструктор
    Option(const Option<T>& other) {
        if (other.value != nullptr) {
            value = new T(*(other.value));
        } else {
            value = nullptr;
        }
    }

    Option<T>& operator=(const Option<T>& other) {
        if (this != &other) {
            delete value; // Очищаем старое значение
            if (other.value != nullptr) {
                value = new T(*(other.value));
            } else {
                value = nullptr;
            }
        }
        return *this;
    }

    ~Option() {
        delete value;
    }

    // Проверка наличия значения
    bool IsSome() const { return value != nullptr; }
    bool IsNone() const { return value == nullptr; }

    // Безопасное извлечение значения
    T GetValue() const {
        if (value == nullptr) {
            throw Exception("Error: Trying to unwrap Option::None!");
        }
        return *value;
    }
};

#endif // OPTION_HPP