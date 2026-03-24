template <class T>
void Sequence<T>::NormalizeSliceParameters(int length, int& index, int& count) const {
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
}

template <class T>
Option<T> Sequence<T>::TryGetFirst() const {
    try {
        return Option<T>(this->GetFirst());
    } catch (const Exception&) {
        return Option<T>();
    }
}

template <class T>
Option<T> Sequence<T>::TryGetLast() const {
    try {
        return Option<T>(this->GetLast());
    } catch (const Exception&) {
        return Option<T>();
    }
}

template <class T>
Option<T> Sequence<T>::TryGet(int index) const {
    try {
        return Option<T>(this->Get(index));
    } catch (const Exception&) {
        return Option<T>();
    }
}

template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq) {
    os << "[";
    IEnumerator<T>* seq_iter = seq.GetEnumerator();
    int i = 0;
    while (seq_iter->MoveNext()) {
        os << seq_iter->GetCurrent();
        if (i < seq.GetLength() - 1) {
            os << ", ";
        }
        i++;
    }
    os << "]";
    delete seq_iter;
    return os;
}