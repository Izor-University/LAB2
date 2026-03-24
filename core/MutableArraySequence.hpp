#ifndef MUTABLE_ARRAY_SEQUENCE_HPP
#define MUTABLE_ARRAY_SEQUENCE_HPP

#include "ArraySequence.hpp"
#include "SequenceBuilders.hpp"

template <class T>
class MutableArraySequence : public ArraySequence<T> {
protected:
    virtual ArraySequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }

    virtual ArraySequence<T>* Instance() override {
        return this;
    }

public:
    // --- ВОЗВРАЩАЕМ СТРОИТЕЛЯ ---
    virtual ISequenceBuilder<T>* CreateBuilder() const override {
        return new ArraySequenceBuilder<T, MutableArraySequence<T>>();
    }

    MutableArraySequence() : ArraySequence<T>() {}
    MutableArraySequence(T* items, int count) : ArraySequence<T>(items, count) {}
    MutableArraySequence(const LinkedList<T>& list) : ArraySequence<T>(list) {}
    MutableArraySequence(const ArraySequence<T>& seq) : ArraySequence<T>(seq) {}

    virtual ~MutableArraySequence() {}
};

#endif // MUTABLE_ARRAY_SEQUENCE_HPP