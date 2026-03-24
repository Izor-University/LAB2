#ifndef MUTABLE_LIST_SEQUENCE_HPP
#define MUTABLE_LIST_SEQUENCE_HPP

#include "ListSequence.hpp"
#include "SequenceBuilders.hpp"

template <class T>
class MutableListSequence : public ListSequence<T> {
protected:
    virtual ListSequence<T>* CreateEmpty() const override {
        return new MutableListSequence<T>();
    }

    virtual ListSequence<T>* Instance() override {
        return this;
    }

public:
    // --- ВОЗВРАЩАЕМ СТРОИТЕЛЯ ---
    virtual ISequenceBuilder<T>* CreateBuilder() const override {
        return new ListSequenceBuilder<T, MutableListSequence<T>>();
    }

    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(T* items, int count) : ListSequence<T>(items, count) {}
    MutableListSequence(const LinkedList<T>& list) : ListSequence<T>(list) {}
    MutableListSequence(const ListSequence<T>& seq) : ListSequence<T>(seq) {}

    virtual ~MutableListSequence() {}
};

#endif // MUTABLE_LIST_SEQUENCE_HPP