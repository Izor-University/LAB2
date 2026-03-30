// --- Операции ---
template <class T>
Sequence<T>* Sequence<T>::Slice(int index, int count, Sequence<T>* insertSeq) {
    int len = this->GetLength();
    int start = (index < 0) ? (len + index) : index;

    if (start < 0 || start > len) {
        throw IndexOutOfRange("Slice: Out of bounds");
    }

    if (count < 0) {
        count = 0;
    }

    if (start + count > len) {
        count = len - start;
    }

    ISequenceBuilder<T>* builder = this->CreateBuilder();

    for (int i = 0; i < len; ++i) {
        if (i == start && insertSeq != nullptr) {
            for (int j = 0; j < insertSeq->GetLength(); ++j) {
                builder->Append(insertSeq->Get(j));
            }
        }

        if (i < start || i >= start + count) {
            builder->Append(this->Get(i));
        }
    }

    if (start == len && insertSeq != nullptr) {
        for (int j = 0; j < insertSeq->GetLength(); ++j) {
            builder->Append(insertSeq->Get(j));
        }
    }

    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

// --- Функциональные методы: Map, Where, Reduce ---
template <class T>
Sequence<T>* Sequence<T>::Map(T (*mapper)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    for (int i = 0; i < this->GetLength(); ++i) {
        builder->Append(mapper(this->Get(i)));
    }
    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
Sequence<T>* Sequence<T>::Where(bool (*predicate)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    for (int i = 0; i < this->GetLength(); ++i) {
        if (predicate(this->Get(i))) {
            builder->Append(this->Get(i));
        }
    }
    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
T Sequence<T>::Reduce(T (*reduce_func)(const T& accumulator, const T& current), const T& start_element) const {
    T result = start_element;
    for (int i = 0; i < this->GetLength(); ++i) {
        result = reduce_func(result, this->Get(i));
    }
    return result;
}

// --- Декомпозиция ---
template <class T>
Sequence<T>* Sequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    int len = this->GetLength();
    if (startIndex < 0 || startIndex >= len || endIndex < 0 || endIndex >= len || startIndex > endIndex) {
        throw IndexOutOfRange("GetSubsequence: Invalid indices");
    }

    ISequenceBuilder<T>* builder = this->CreateBuilder();
    for (int i = startIndex; i <= endIndex; ++i) {
        builder->Append(this->Get(i));
    }

    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
Sequence<T>* Sequence<T>::Concat(Sequence<T>* list) const {
    if (list == nullptr) {
        return this->clone();
    }

    ISequenceBuilder<T>* builder = this->CreateBuilder();

    for (int i = 0; i < this->GetLength(); ++i) {
        builder->Append(this->Get(i));
    }

    for (int i = 0; i < list->GetLength(); ++i) {
        builder->Append(list->Get(i));
    }

    Sequence<T>* result = builder->Build();
    delete builder;
    return result;
}

template <class T>
const T& Sequence<T>::GetFirst() const {
    if (this->GetLength() == 0) {
        throw EmptyCollectionError("Sequence is empty");
    }
    return this->Get(0);
}

template <class T>
const T& Sequence<T>::GetLast() const {
    if (this->GetLength() == 0) {
        throw EmptyCollectionError("Sequence is empty");
    }
    return this->Get(this->GetLength() - 1);
}

// --- Try-семантика ---
template <class T>
Option<T> Sequence<T>::TryGet(int index) const {
    if (index < 0 || index >= this->GetLength()) {
        return Option<T>();
    }
    return Option<T>(this->Get(index));
}

template <class T>
Option<T> Sequence<T>::TryGetFirst() const {
    return this->TryGet(0);
}

template <class T>
Option<T> Sequence<T>::TryGetLast() const {
    return this->TryGet(this->GetLength() - 1);
}

// --- Перегрузка оператора вывода ---
template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq) {
    os << "[";
    for (int i = 0; i < seq.GetLength(); ++i) {
        os << seq.Get(i);
        if (i < seq.GetLength() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}