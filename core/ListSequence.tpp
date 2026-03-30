// --- Конструкторы и деструктор ---
template <class T>
ListSequence<T>::ListSequence() {
    items = new LinkedList<T>();
}

template <class T>
ListSequence<T>::ListSequence(T* arr, int count) {
    items = new LinkedList<T>(arr, count);
}

template <class T>
ListSequence<T>::ListSequence(const LinkedList<T>& list) {
    items = new LinkedList<T>(list);
}

template <class T>
ListSequence<T>::ListSequence(const ListSequence<T>& seq) {
    items = new LinkedList<T>(*(seq.items));
}

template <class T>
ListSequence<T>::~ListSequence() {
    delete items;
}

// --- Декомпозиция ---
template <class T>
const T& ListSequence<T>::Get(int index) const {
    return items->Get(index);
}

template <class T>
int ListSequence<T>::GetLength() const {
    return items->GetLength();
}

// --- Внутренние методы модификации ---
template <class T>
Sequence<T>* ListSequence<T>::AppendInternal(const T& item) {
    items->Append(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::PrependInternal(const T& item) {
    items->Prepend(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAtInternal(const T& item, int index) {
    items->InsertAt(item, index);
    return this;
}

// --- Основные операции интерфейса ---
template <class T>
Sequence<T>* ListSequence<T>::Append(const T& item) {
    return this->Instance()->AppendInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::Prepend(const T& item) {
    return this->Instance()->PrependInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAt(const T& item, int index) {
    return this->Instance()->InsertAtInternal(item, index);
}

template <class T>
const T& ListSequence<T>::operator[](int index) const {
    return this->Get(index);
}