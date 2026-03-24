#ifndef IMMUTABLE_ARRAY_SEQUENCE_HPP
#define IMMUTABLE_ARRAY_SEQUENCE_HPP

#include "ArraySequence.hpp"
#include "SequenceBuilders.hpp"

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    virtual ArraySequence<T>* CreateEmpty() const override {
        return new ImmutableArraySequence<T>();
    }

    virtual ArraySequence<T>* Instance() override {
        return new ImmutableArraySequence<T>(*this);
    }

public:
    // --- ВОЗВРАЩАЕМ СТРОИТЕЛЯ ---
    virtual ISequenceBuilder<T>* CreateBuilder() const override {
        return new ArraySequenceBuilder<T, ImmutableArraySequence<T>>();
    }

    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(T* items, int count) : ArraySequence<T>(items, count) {}
    ImmutableArraySequence(const LinkedList<T>& list) : ArraySequence<T>(list) {}
    ImmutableArraySequence(const ArraySequence<T>& seq) : ArraySequence<T>(seq) {}

    virtual ~ImmutableArraySequence() {}
};

#endif // IMMUTABLE_ARRAY_SEQUENCE_HPP