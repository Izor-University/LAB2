#ifndef IENABLE_HPP
#define IENABLE_HPP

#include "IEnumerator.hpp"

template <class T>
class IEnumerable {
public:
    virtual ~IEnumerable() {}

    // Фабричный метод для создания итератора
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

#endif // IENABLE_HPP