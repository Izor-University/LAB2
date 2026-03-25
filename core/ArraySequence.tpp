// Конструкторы и деструктор
template <class T>
ArraySequence<T>::ArraySequence() { items = new DynamicArray<T>(0); }

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
const T& ArraySequence<T>::GetFirst() const {
    if (items->GetSize() == 0) throw EmptyCollectionError("ArraySequence is empty!");
    return items->Get(0);
}

template <class T>
const T& ArraySequence<T>::GetLast() const {
    if (items->GetSize() == 0) throw EmptyCollectionError("ArraySequence is empty!");
    return items->Get(items->GetSize() - 1);
}

template <class T>
const T& ArraySequence<T>::Get(int index) const { return items->Get(index); }

template <class T>
int ArraySequence<T>::GetLength() const { return items->GetSize(); }

// Внутренние методы модификации
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

template <class T>
Sequence<T>* ArraySequence<T>::SliceInternal(int index, int count, Sequence<T>* insertSequence) {
    int length = this->GetLength();
    this->NormalizeSliceParameters(length, index, count);

    int insertLength = 0;
    if (insertSequence != nullptr) insertLength = insertSequence->GetLength();

    DynamicArray<T>* newArray = new DynamicArray<T>(0);
    newArray->Resize(length - count + insertLength);
    int position = 0;

    for (int i = 0; i < index; ++i) { newArray->Set(position++, this->Get(i)); }
    if (insertSequence != nullptr) {
        for (int i = 0; i < insertLength; ++i) { newArray->Set(position++, insertSequence->Get(i)); }
    }
    for (int i = index + count; i < length; ++i) { newArray->Set(position++, this->Get(i)); }

    delete this->items;
    this->items = newArray;
    return this;
}

// Основные операции интерфейса
template <class T>
Sequence<T>* ArraySequence<T>::Append(const T& item) { return this->Instance()->AppendInternal(item); }

template <class T>
Sequence<T>* ArraySequence<T>::Prepend(const T& item) { return this->Instance()->PrependInternal(item); }

template <class T>
Sequence<T>* ArraySequence<T>::InsertAt(const T& item, int index) { return this->Instance()->InsertAtInternal(item, index); }

template <class T>
Sequence<T>* ArraySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= this->GetLength() || startIndex > endIndex) throw IndexOutOfRange("Invalid indices");
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    for (int i = startIndex; i <= endIndex; ++i) builder->Append(this->Get(i));
    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
Sequence<T>* ArraySequence<T>::Concat(Sequence<T>* list) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    for (int i = 0; i < this->GetLength(); ++i) builder->Append(this->Get(i));
    for (int i = 0; i < list->GetLength(); ++i) builder->Append(list->Get(i));
    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
const T& ArraySequence<T>::operator[](int index) const { return this->Get(index); }

template <class T>
Sequence<T>* ArraySequence<T>::map(T (*mapper)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    for (int i = 0; i < this->GetLength(); ++i) builder->Append(mapper(this->Get(i)));
    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
Sequence<T>* ArraySequence<T>::where(bool (*predicate)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    for (int i = 0; i < this->GetLength(); ++i) {
        if (predicate(this->Get(i))) builder->Append(this->Get(i));
    }
    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
T ArraySequence<T>::reduce(T (*reduce_func)(const T& first_element, const T& second_element), const T& start_element) const {
    T result = start_element;
    for (int i = 0; i < this->GetLength(); ++i) result = reduce_func(result, this->Get(i));
    return result;
}

template <class T>
Sequence<T>* ArraySequence<T>::Slice(int index, int count, Sequence<T>* insertSequence) {
    return this->Instance()->SliceInternal(index, count, insertSequence);
}