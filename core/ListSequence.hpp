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
        const T& GetCurrent() const override { return seq->Get(currentIndex); }
        void Reset() override { currentIndex = -1; }
    };

protected:
    LinkedList<T>* items;
    virtual ListSequence<T>* Instance() = 0;

    Sequence<T>* AppendInternal(const T& item);
    Sequence<T>* PrependInternal(const T& item);
    Sequence<T>* InsertAtInternal(const T& item, int index);
    Sequence<T>* SliceInternal(int index, int count, Sequence<T>* insertSeq);

public:
    ListSequence();
    ListSequence(T* items, int count);
    ListSequence(const LinkedList<T>& list);
    ListSequence(const ListSequence<T>& seq);
    virtual ~ListSequence();

    IEnumerator<T>* GetEnumerator() const override { return new ListEnumerator(this); }

    virtual const T& GetFirst() const override;
    virtual const T& GetLast() const override;
    virtual const T& Get(int index) const override;
    virtual int GetLength() const override;
    virtual Sequence<T>* Append(const T& item) override;
    virtual Sequence<T>* Prepend(const T& item) override;
    virtual Sequence<T>* InsertAt(const T& item, int index) override;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    virtual Sequence<T>* Concat(Sequence<T>* list) const override;
    virtual const T& operator[](int index) const override;
    virtual Sequence<T>* map(T (*mapper)(const T& element)) const override;
    virtual Sequence<T>* where(bool (*predicate)(const T& element)) const override;
    virtual T reduce(T (*reduce_func)(const T& first_element, const T& second_element), const T& start_element) const override;
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* insertSeq = nullptr) override;
};

#include "ListSequence.tpp"
#endif