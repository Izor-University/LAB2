// Конструкторы и деструктор
template <class T>
ArraySequence<T>::ArraySequence() { items = new DynamicArray<T>(); }

template <class T>
ArraySequence<T>::ArraySequence(T* arr, int count) { items = new DynamicArray<T>(arr, count); }

template <class T>
ArraySequence<T>::ArraySequence(const LinkedList<T>& list) {
    int length = list.GetLength();
    items = new DynamicArray<T>(length);
    for (int i = 0; i < length; ++i) items->Set(i, list.Get(i));
}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& seq) { items = new DynamicArray<T>(*(seq.items)); }

template <class T>
ArraySequence<T>::~ArraySequence() { delete items; }

// Декомпозиция
template <class T>
const T& ArraySequence<T>::Get(int index) const { return items->Get(index); }

template <class T>
int ArraySequence<T>::GetLength() const { return items->GetSize(); }

// Внутренние методы модификации (для паттерна Шаблонный метод)
template <class T>
Sequence<T>* ArraySequence<T>::AppendInternal(const T& item) {
    int size = items->GetSize();
    items->Resize(size + 1);
    items->Set(size, item);
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::PrependInternal(const T& item) {
    int size = items->GetSize();
    items->Resize(size + 1);
    for (int i = size; i > 0; --i) items->Set(i, items->Get(i - 1));
    items->Set(0, item);
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAtInternal(const T& item, int index) {
    int size = items->GetSize();
    if (index < 0 || index > size) throw IndexOutOfRange("Invalid index in InsertAt");
    items->Resize(size + 1);
    for (int i = size; i > index; --i) items->Set(i, items->Get(i - 1));
    items->Set(index, item);
    return this;
}

// Публичные методы модификации
template <class T>
Sequence<T>* ArraySequence<T>::Append(const T& item) { return this->Instance()->AppendInternal(item); }

template <class T>
Sequence<T>* ArraySequence<T>::Prepend(const T& item) { return this->Instance()->PrependInternal(item); }

template <class T>
Sequence<T>* ArraySequence<T>::InsertAt(const T& item, int index) { return this->Instance()->InsertAtInternal(item, index); }

template <class T>
const T& ArraySequence<T>::operator[](int index) const { return this->Get(index); }