// Конструктор по умолчанию
template <class T>
DynamicArray<T>::DynamicArray() : data(nullptr), size(0), capacity(0) {}

// Конструктор с заданным размером
template <class T>
DynamicArray<T>::DynamicArray(int size) : size(size), capacity(size) {
    if (size < 0) {
        throw IndexOutOfRange("Error: Size cannot be negative!");
    }
    data = (this->capacity > 0) ? new T[this->capacity]() : nullptr;
}

// Конструктор из обычного массива
template <class T>
DynamicArray<T>::DynamicArray(T* items, int count) : size(count), capacity(count) {
    if (count < 0) {
        throw IndexOutOfRange("Error: Count cannot be negative!");
    }
    data = (this->capacity > 0) ? new T[this->capacity]() : nullptr;
    for (int i = 0; i < this->size; ++i) {
        data[i] = items[i];
    }
}

// Копирующий конструктор
template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) : size(other.size), capacity(other.capacity) {
    data = (this->capacity > 0) ? new T[this->capacity]() : nullptr;
    for (int i = 0; i < this->size; ++i) {
        data[i] = other.data[i];
    }
}

// Move-конструктор (Перемещение без выделения новой памяти)
template <class T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& other) noexcept
    : data(other.data), size(other.size), capacity(other.capacity) {
    // "Крадем" данные и обнуляем источник
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

// Деструктор
template <class T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

// Копирующий оператор присваивания (Паттерн Copy-and-Swap)
// Дает строгую гарантию исключений (Strong exception guarantee)
template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this != &other) {
        // Выделение памяти происходит здесь. Если new выбросит bad_alloc,
        // наш текущий объект не сломается, так как мы еще ничего не меняли.
        DynamicArray<T> temp(other);

        // Ручной Swap без использования STL (<algorithm>)
        T* tempData = data;
        data = temp.data;
        temp.data = tempData;

        int tempSize = size;
        size = temp.size;
        temp.size = tempSize;

        int tempCapacity = capacity;
        capacity = temp.capacity;
        temp.capacity = tempCapacity;

        // Старая память удалится автоматически при вызове деструктора temp
    }
    return *this;
}

// Move-оператор присваивания
template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& other) noexcept {
    if (this != &other) {
        delete[] data; // Очищаем свою память

        // Забираем данные у other
        data = other.data;
        size = other.size;
        capacity = other.capacity;

        // Обнуляем other
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }
    return *this;
}

// Декомпозиция
template <class T>
const T& DynamicArray<T>::Get(int index) const {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Get()!");
    }
    return data[index];
}

template <class T>
int DynamicArray<T>::GetSize() const {
    return size;
}

template <class T>
int DynamicArray<T>::GetCapacity() const {
    return capacity;
}

// Операции
template <class T>
void DynamicArray<T>::Set(int index, const T& value) {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Set()!");
    }
    data[index] = value;
}

template <class T>
void DynamicArray<T>::RemoveAt(int index) {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in RemoveAt()!");
    }
    for (int i = index; i < size - 1; ++i) {
        data[i] = data[i + 1];
    }
    size--; // Физически память не удаляем, просто скрываем элемент
}

// Умный Resize с поддержкой capacity
template <class T>
void DynamicArray<T>::Resize(int newSize) {
    if (newSize < 0) {
        throw IndexOutOfRange("Error: New size cannot be negative!");
    }

    // Если памяти уже хватает, просто меняем логический размер
    if (newSize <= capacity) {
        // Если размер увеличивается, инициализируем новые элементы пустыми значениями
        if (newSize > size) {
            for (int i = size; i < newSize; ++i) {
                data[i] = T();
            }
        }
        size = newSize;
        return;
    }

    // Если памяти не хватает, удваиваем текущую вместимость (Амортизированное O(1))
    int newCapacity = (capacity == 0) ? 1 : capacity * 2;
    if (newCapacity < newSize) {
        newCapacity = newSize; // На случай, если пользователь запросил сразу очень много
    }

    // Выделяем новую память с запасом
    T* newData = new T[newCapacity]();

    // Копируем существующие элементы
    for (int i = 0; i < size; ++i) {
        newData[i] = data[i];
    }

    // Обновляем состояние
    delete[] data;
    data = newData;
    size = newSize;
    capacity = newCapacity;
}