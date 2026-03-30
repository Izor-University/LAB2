#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include "Exceptions.hpp"

template <class T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;

public:
    // --- Конструкторы ---
    DynamicArray();
    explicit DynamicArray(int size);
    DynamicArray(T* items, int count);

    // Копирующий конструктор
    DynamicArray(const DynamicArray<T>& other);

    // Move-конструктор (конструктор перемещения)
    DynamicArray(DynamicArray<T>&& other) noexcept;

    // Деструктор
    ~DynamicArray();

    // --- Операторы присваивания ---
    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(DynamicArray<T>&& other) noexcept;

    // --- Декомпозиция ---
    const T& Get(int index) const;
    int GetSize() const;
    int GetCapacity() const;

    // --- Операции ---
    void Set(int index, const T& value);
    void Resize(int newSize);
    void RemoveAt(int index);
};

#include "DynamicArray.tpp"
#endif // DYNAMIC_ARRAY_HPP