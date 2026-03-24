#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include "exceptions.hpp"

template <class T>
class DynamicArray {
private:
    T* data;
    int size;

public:
    // Создание объекта
    DynamicArray(T* items, int count);
    explicit DynamicArray(int size);
    DynamicArray(const DynamicArray<T>& dynamicArray);

    // Деструктор
    ~DynamicArray();

    // Декомпозиция
    const T& Get(int index) const;
    int GetSize() const;

    // Операции
    void Set(int index, const T& value);
    void Resize(int newSize);
};

// Подключение реализации шаблонных методов
#include "DynamicArray.tpp"

#endif // DYNAMIC_ARRAY_HPP