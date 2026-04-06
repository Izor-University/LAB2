// --- Конструкторы и деструктор ---
template <class T>
ArraySequence<T>::ArraySequence() {
    items = new DynamicArray<T>();
}

template <class T>
ArraySequence<T>::ArraySequence(T* arr, int count) {
    items = new DynamicArray<T>(arr, count);
}

template <class T>
ArraySequence<T>::ArraySequence(const LinkedList<T>& list) {
    int length = list.GetLength();
    items = new DynamicArray<T>(length);
    for (int i = 0; i < length; ++i) {
        items->Set(i, list.Get(i));
    }
}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& seq) {
    items = new DynamicArray<T>(*(seq.items));
}

template <class T>
ArraySequence<T>::~ArraySequence() {
    delete items;
}

// --- Декомпозиция ---
template <class T>
const T& ArraySequence<T>::Get(int index) const {
    return items->Get(index);
}

template <class T>
int ArraySequence<T>::GetLength() const {
    return items->GetSize();
}

// --- Оптимизированные операции ---
template <class T>
Sequence<T>* ArraySequence<T>::Concat(Sequence<T>* list) const {
    // 1. Клонируем текущую последовательность (сразу копируется весь наш массив)
    ArraySequence<T>* result = static_cast<ArraySequence<T>*>(this->clone());

    if (list != nullptr) {
        int oldSize = result->GetLength();
        int addedSize = list->GetLength();

        // ОПТИМИЗАЦИЯ: Расширяем массив всего ОДИН РАЗ под итоговый размер!
        result->items->Resize(oldSize + addedSize);

        IEnumerator<T>* en = list->GetEnumerator();
        int i = oldSize;
        try {
            while (en->MoveNext()) {
                // Прямая вставка по индексу (без вызовов Append и Resize)
                result->items->Set(i++, en->GetCurrent());
            }
        } catch (...) {
            delete en;
            delete result;
            throw;
        }
        delete en;
    }
    return result;
}

template <class T>
Sequence<T>* ArraySequence<T>::Slice(int index, int count, Sequence<T>* insertSeq) {
    int len = this->GetLength();
    int start = (index < 0) ? (len + index) : index;
    if (start < 0 || start > len) throw IndexOutOfRange("Slice: Out of bounds");
    if (count < 0) count = 0;
    if (start + count > len) count = len - start;

    int insertLen = (insertSeq != nullptr) ? insertSeq->GetLength() : 0;

    // Вычисляем точный размер будущего массива
    int newSize = len - count + insertLen;

    // Создаем пустую последовательность нужного типа (Mutable/Immutable)
    ArraySequence<T>* result = static_cast<ArraySequence<T>*>(this->create_empty());

    // ОПТИМИЗАЦИЯ: Выделяем память один раз!
    result->items->Resize(newSize);

    int destIdx = 0;
    // 1. Копируем первую часть (до start)
    for (int i = 0; i < start; ++i) {
        result->items->Set(destIdx++, this->Get(i));
    }

    // 2. Вставляем элементы из insertSeq
    if (insertSeq != nullptr) {
        IEnumerator<T>* en = insertSeq->GetEnumerator();
        try {
            while (en->MoveNext()) {
                result->items->Set(destIdx++, en->GetCurrent());
            }
        } catch (...) {
            delete en;
            delete result;
            throw;
        }
        delete en;
    }

    // 3. Копируем остаток хвоста
    for (int i = start + count; i < len; ++i) {
        result->items->Set(destIdx++, this->Get(i));
    }

    return result;
}
// --- Внутренние методы модификации ---
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
    for (int i = size; i > 0; --i) {
        items->Set(i, items->Get(i - 1));
    }
    items->Set(0, item);
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAtInternal(const T& item, int index) {
    int size = items->GetSize();
    if (index < 0 || index > size) {
        throw IndexOutOfRange("Invalid index in InsertAt");
    }
    items->Resize(size + 1);
    for (int i = size; i > index; --i) {
        items->Set(i, items->Get(i - 1));
    }
    items->Set(index, item);
    return this;
}

// --- Публичные методы модификации ---
template <class T>
Sequence<T>* ArraySequence<T>::Append(const T& item) {
    return this->Instance()->AppendInternal(item);
}

template <class T>
Sequence<T>* ArraySequence<T>::Prepend(const T& item) {
    return this->Instance()->PrependInternal(item);
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAt(const T& item, int index) {
    return this->Instance()->InsertAtInternal(item, index);
}

template <class T>
const T& ArraySequence<T>::operator[](int index) const {
    return this->Get(index);
}

template <class T>
ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T>& other) {
    if (this != &other) {
        *(this->items) = *(other.items);
    }
    return *this;
}