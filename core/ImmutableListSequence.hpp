#ifndef IMMUTABLE_LIST_SEQUENCE_HPP
#define IMMUTABLE_LIST_SEQUENCE_HPP

#include "ListSequence.hpp"
#include "SequenceBuilders.hpp"

template <class T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    virtual ListSequence<T>* CreateEmpty() const override {
        return new ImmutableListSequence<T>();
    }

    virtual ListSequence<T>* Instance() override {
        return new ImmutableListSequence<T>(*this);
    }

public:
    // --- ВОЗВРАЩАЕМ СТРОИТЕЛЯ ---
    virtual ISequenceBuilder<T>* CreateBuilder() const override {
        return new ListSequenceBuilder<T, ImmutableListSequence<T>>();
    }

    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T* items, int count) : ListSequence<T>(items, count) {}
    ImmutableListSequence(const LinkedList<T>& list) : ListSequence<T>(list) {}
    ImmutableListSequence(const ListSequence<T>& seq) : ListSequence<T>(seq) {}

    virtual ~ImmutableListSequence() {}
};

#endif // IMMUTABLE_LIST_SEQUENCE_HPP