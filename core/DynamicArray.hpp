#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include "exceptions.hpp"

template <class T>
class DynamicArray {
private:
    T* data;    // Указатель на выделенную память
    int size;   // Текущий размер массива

public:
    // Создание объекта
    DynamicArray(T* items, int count);                           // Копировать элементы из переданного массива [cite: 82]
    explicit DynamicArray(int size);                             // Создать массив заданной длины [cite: 82]
    DynamicArray(const DynamicArray<T>& dynamicArray);           // Копирующий конструктор [cite: 82]

    // Деструктор (обязателен для предотвращения утечек памяти)
    ~DynamicArray();

    // Декомпозиция
    T Get(int index) const;                                      // Получить элемент по индексу [cite: 85, 91]
    int GetSize() const;                                         // Получить размер массива [cite: 88, 92]

    // Операции
    void Set(int index, T value);                                // Задать элемент по индексу [cite: 90, 94]
    void Resize(int newSize);                                    // Изменить размер массива [cite: 93, 98]
};

// Подключение реализации шаблонных методов
#include "DynamicArray.tpp"

#endif // DYNAMIC_ARRAY_HPP