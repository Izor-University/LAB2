#ifndef LIST_SEQUENCE_HPP
#define LIST_SEQUENCE_HPP

#include "Sequence.hpp"
#include "LinkedList.hpp"

template <class T>
class ListSequence : public Sequence<T> {
private:
    class ListEnumerator : public IEnumerator<T> {
    private:
        const ListSequence<T>* seq;
        int currentIndex;
    public:
        ListEnumerator(const ListSequence<T>* s) : seq(s), currentIndex(-1) {}

        bool MoveNext() override {
            if (currentIndex + 1 < seq->GetLength()) {
                currentIndex++;
                return true;
            }
            return false;
        }

        T GetCurrent() const override {
            return seq->Get(currentIndex);
        }

        void Reset() override { currentIndex = -1; }
    };

protected:
    LinkedList<T>* items;
    virtual ListSequence<T>* CreateEmpty() const = 0;
    virtual ListSequence<T>* Instance() = 0;

    Sequence<T>* AppendInternal(T item);
    Sequence<T>* PrependInternal(T item);
    Sequence<T>* InsertAtInternal(T item, int index);
    Sequence<T>* SliceInternal(int index, int count, Sequence<T>* insertSeq);

public:
    ListSequence();
    ListSequence(T* items, int count);
    ListSequence(const LinkedList<T>& list);
    ListSequence(const ListSequence<T>& seq);
    virtual ~ListSequence();

    // Реализация IEnumerable
    IEnumerator<T>* GetEnumerator() const override {
        return new ListEnumerator(this);
    }

    // Все остальные методы остаются (GetLength, Get, TryGet и т.д.)
    virtual T GetFirst() const override;
    virtual T GetLast() const override;
    virtual T Get(int index) const override;
    virtual int GetLength() const override;
    virtual Sequence<T>* Append(T item) override;
    virtual Sequence<T>* Prepend(T item) override;
    virtual Sequence<T>* InsertAt(T item, int index) override;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    virtual Sequence<T>* Concat(Sequence<T>* list) const override;
    virtual T operator[](int index) const override;
    virtual Option<T> TryGetFirst() const override;
    virtual Option<T> TryGetLast() const override;
    virtual Option<T> TryGet(int index) const override;
    virtual Sequence<T>* map(T (*mapper)(const T& element)) const override;
    virtual Sequence<T>* where(bool (*predicate)(const T& element)) const override;
    virtual T reduce(T (*reduce_func)(const T& first_element, const T& second_element), const T& start_element) const override;
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* insertSeq = nullptr) override;
};

#include "ListSequence.tpp"
#endif