#ifndef ENUMERATOR_HPP
#define ENUMERATOR_HPP

#include "Option.hpp"

// Интерфейс итератора
template <class T>
class IEnumerator {
public:
    virtual ~IEnumerator() {}

    // Переход к следующему элементу. Возвращает false, если достигнут конец.
    virtual bool MoveNext() = 0;

    // Получение текущего элемента
    virtual const T& GetCurrent() const = 0;

    // Сброс итератора в начало
    virtual void Reset() = 0;
};

// Интерфейс перечисляемого объекта
template <class T>
class IEnumerable {
public:
    virtual ~IEnumerable() {}

    // Фабричный метод для создания итератора
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

#endif // ENUMERATOR_HPP