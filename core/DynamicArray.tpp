// Этот файл не должен иметь Include Guards или подключаться напрямую в main.cpp
// Он автоматически включается в конце DynamicArray.hpp

template <class T>
DynamicArray<T>::DynamicArray(T* items, int count) {
    if (count < 0) {
        throw IndexOutOfRange("Error: Count cannot be negative!");
    }
    size = count;
    data = new T[size];
    for (int i = 0; i < size; ++i) {
        data[i] = items[i];
    }
}

template <class T>
DynamicArray<T>::DynamicArray(int size) {
    if (size < 0) {
        throw IndexOutOfRange("Error: Size cannot be negative!");
    }
    this->size = size;
    // Выделяем память и инициализируем элементы значениями по умолчанию
    data = new T[this->size]();
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& dynamicArray) {
    size = dynamicArray.size;
    data = new T[size];
    for (int i = 0; i < size; ++i) {
        data[i] = dynamicArray.data[i];
    }
}

template <class T>
DynamicArray<T>::~DynamicArray() {
    delete[] data; // Освобождаем память
}

template <class T>
T DynamicArray<T>::Get(int index) const {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Get()!"); // Может выбрасывать исключения [cite: 86, 87]
    }
    return data[index];
}

template <class T>
int DynamicArray<T>::GetSize() const {
    return size;
}

template <class T>
void DynamicArray<T>::Set(int index, T value) {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Set()!"); // Может выбрасывать исключения [cite: 95, 96]
    }
    data[index] = value;
}

template <class T>
void DynamicArray<T>::Resize(int newSize) {
    if (newSize < 0) {
        throw IndexOutOfRange("Error: New size cannot be negative!");
    }

    if (newSize == size) {
        return; // Ничего не делаем, если размер не меняется
    }

    // Выделяем новую память
    T* newData = new T[newSize]();

    // Копируем элементы. Если размер уменьшается - лишние отбрасываются
    // Если увеличивается - копируются в начало
    int elementsToCopy = (newSize < size) ? newSize : size;
    for (int i = 0; i < elementsToCopy; ++i) {
        newData[i] = data[i];
    }

    // Удаляем старую память и переназначаем указатели
    delete[] data;
    data = newData;
    size = newSize;
}