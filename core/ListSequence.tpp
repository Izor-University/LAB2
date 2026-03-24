// Конструкторы и деструктор
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

// Декомпозиция
template <class T>
T ListSequence<T>::GetFirst() const {
    if (items->GetLength() == 0) {
        throw EmptyCollectionError("ListSequence is empty!");
    }
    return items->GetFirst();
}

template <class T>
T ListSequence<T>::GetLast() const {
    if (items->GetLength() == 0) {
        throw EmptyCollectionError("ListSequence is empty!");
    }
    return items->GetLast();
}

template <class T>
T ListSequence<T>::Get(int index) const {
    return items->Get(index);
}

template <class T>
int ListSequence<T>::GetLength() const {
    return items->GetLength();
}

// Внутренние методы модификации
template <class T>
Sequence<T>* ListSequence<T>::AppendInternal(T item) {
    items->Append(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::PrependInternal(T item) {
    items->Prepend(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAtInternal(T item, int index) {
    items->InsertAt(item, index);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::SliceInternal(int index, int count, Sequence<T>* insertSequence) {
    int length = this->GetLength();

    if (index < 0) {
        index = length + index;
    }
    if (index < 0 || index > length) {
        throw IndexOutOfRange("Slice: index out of bounds");
    }
    if (count < 0) {
        count = 0;
    }
    if (index + count > length) {
        count = length - index;
    }

    LinkedList<T>* newLinkedList = new LinkedList<T>();
    int insertLength = 0;
    if (insertSequence != nullptr) {
        insertLength = insertSequence->GetLength();
    }

    for (int i = 0; i < index; ++i) {
        newLinkedList->Append(this->Get(i));
    }

    if (insertSequence != nullptr) {
        for (int i = 0; i < insertLength; ++i) {
            newLinkedList->Append(insertSequence->Get(i));
        }
    }

    for (int i = index + count; i < length; ++i) {
        newLinkedList->Append(this->Get(i));
    }

    delete this->items;
    this->items = newLinkedList;
    return this;
}

// Основные операции интерфейса
template <class T>
Sequence<T>* ListSequence<T>::Append(T item) {
    return this->Instance()->AppendInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::Prepend(T item) {
    return this->Instance()->PrependInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAt(T item, int index) {
    return this->Instance()->InsertAtInternal(item, index);
}

template <class T>
Sequence<T>* ListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    LinkedList<T>* subList = items->GetSubList(startIndex, endIndex);
    ListSequence<T>* newSequence = this->CreateEmpty();
    delete newSequence->items;
    newSequence->items = subList;
    return newSequence;
}

template <class T>
Sequence<T>* ListSequence<T>::Concat(Sequence<T>* list) const {
    ListSequence<T>* newSequence = this->CreateEmpty();
    for (int i = 0; i < this->GetLength(); ++i) {
        newSequence->AppendInternal(this->Get(i));
    }
    for (int i = 0; i < list->GetLength(); ++i) {
        newSequence->AppendInternal(list->Get(i));
    }
    return newSequence;
}

// Бонусные методы
template <class T>
T ListSequence<T>::operator[](int index) const {
    return this->Get(index);
}

template <class T>
Option<T> ListSequence<T>::TryGetFirst() const {
    try {
        return Option<T>(this->GetFirst());
    } catch (const Exception&) {
        return Option<T>();
    }
}

template <class T>
Option<T> ListSequence<T>::TryGetLast() const {
    try {
        return Option<T>(this->GetLast());
    } catch (const Exception&) {
        return Option<T>();
    }
}

template <class T>
Option<T> ListSequence<T>::TryGet(int index) const {
    try {
        return Option<T>(this->Get(index));
    } catch (const Exception&) {
        return Option<T>();
    }
}

template <class T>
Sequence<T>* ListSequence<T>::map(T (*mapper)(const T& element)) const {
    ListSequence<T>* result = this->CreateEmpty();
    for (int i = 0; i < this->GetLength(); ++i) {
        result->AppendInternal(mapper(this->Get(i)));
    }
    return result;
}

template <class T>
Sequence<T>* ListSequence<T>::where(bool (*predicate)(const T& element)) const {
    ListSequence<T>* result = this->CreateEmpty();
    for (int i = 0; i < this->GetLength(); ++i) {
        if (predicate(this->Get(i))) {
            result->AppendInternal(this->Get(i));
        }
    }
    return result;
}

template <class T>
T ListSequence<T>::reduce(T (*reduce_func)(const T& first_element, const T& second_element), const T& start_element) const {
    T result = start_element;
    for (int i = 0; i < this->GetLength(); ++i) {
        result = reduce_func(result, this->Get(i));
    }
    return result;
}

template <class T>
Sequence<T>* ListSequence<T>::Slice(int index, int count, Sequence<T>* insertSequence) {
    return this->Instance()->SliceInternal(index, count, insertSequence);
}