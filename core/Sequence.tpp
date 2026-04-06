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
    IEnumerator<T>* en = this->GetEnumerator();

    try {
        int currentIndex = 0;
        while (en->MoveNext()) {
            if (currentIndex == start && insertSeq != nullptr) {
                IEnumerator<T>* insertEn = insertSeq->GetEnumerator();
                try {
                    while (insertEn->MoveNext()) {
                        builder->Append(insertEn->GetCurrent());
                    }
                } catch (...) {
                    delete insertEn;
                    throw;
                }
                delete insertEn;
            }

            if (currentIndex < start || currentIndex >= start + count) {
                builder->Append(en->GetCurrent());
            }
            currentIndex++;
        }

        if (start == len && insertSeq != nullptr) {
            IEnumerator<T>* insertEn = insertSeq->GetEnumerator();
            try {
                while (insertEn->MoveNext()) {
                    builder->Append(insertEn->GetCurrent());
                }
            } catch (...) {
                delete insertEn;
                throw;
            }
            delete insertEn;
        }
    } catch (...) {
        delete en;
        delete builder;
        throw;
    }

    Sequence<T>* result = builder->Build();
    delete en;
    delete builder;
    return result;
}

// --- Функциональные методы: Map, Where, Reduce ---
template <class T>
Sequence<T>* Sequence<T>::Map(T (*mapper)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en = this->GetEnumerator();
    
    try {
        while (en->MoveNext()) {
            builder->Append(mapper(en->GetCurrent()));
        }
    } catch (...) {
        delete en;
        delete builder;
        throw;
    }
    
    Sequence<T>* result = builder->Build();
    delete en;
    delete builder;
    return result;
}

template <class T>
Sequence<T>* Sequence<T>::Where(bool (*predicate)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en = this->GetEnumerator();
    
    try {
        while (en->MoveNext()) {
            if (predicate(en->GetCurrent())) {
                builder->Append(en->GetCurrent());
            }
        }
    } catch (...) {
        delete en;
        delete builder;
        throw;
    }
    
    Sequence<T>* result = builder->Build();
    delete en;
    delete builder;
    return result;
}

template <class T>
T Sequence<T>::Reduce(T (*reduce_func)(const T& accumulator, const T& current), const T& start_element) const {
    T result = start_element;
    IEnumerator<T>* en = this->GetEnumerator();
    
    try {
        while (en->MoveNext()) {
            result = reduce_func(result, en->GetCurrent());
        }
    } catch (...) {
        delete en;
        throw;
    }
    
    delete en;
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
    IEnumerator<T>* en = this->GetEnumerator();
    
    try {
        int currentIndex = 0;
        while (en->MoveNext()) {
            if (currentIndex >= startIndex && currentIndex <= endIndex) {
                builder->Append(en->GetCurrent());
            }
            if (currentIndex > endIndex) {
                break;
            }
            currentIndex++;
        }
    } catch (...) {
        delete en;
        delete builder;
        throw;
    }

    Sequence<T>* result = builder->Build();
    delete en;
    delete builder;
    return result;
}

template <class T>
Sequence<T>* Sequence<T>::Concat(Sequence<T>* list) const {
    if (list == nullptr) {
        return this->clone();
    }

    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en1 = this->GetEnumerator();
    IEnumerator<T>* en2 = list->GetEnumerator();

    try {
        while (en1->MoveNext()) {
            builder->Append(en1->GetCurrent());
        }
        while (en2->MoveNext()) {
            builder->Append(en2->GetCurrent());
        }
    } catch (...) {
        delete en1;
        delete en2;
        delete builder;
        throw;
    }

    Sequence<T>* result = builder->Build();
    delete en1;
    delete en2;
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

