#ifndef IMMUTABLE_ARRAY_SEQUENCE_HPP
#define IMMUTABLE_ARRAY_SEQUENCE_HPP

#include "ArraySequence.hpp"

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    virtual ArraySequence<T>* Instance() override { return new ImmutableArraySequence<T>(*this); }

public:
    class Builder : public ISequenceBuilder<T> {
    private:
        ImmutableArraySequence<T>* seq;
    public:
        Builder() { seq = new ImmutableArraySequence<T>(); }
        virtual ~Builder() { if (seq) delete seq; }

        virtual ISequenceBuilder<T>* Append(const T& item) override {
            seq->AppendInternal(item);
            return this;
        }
        virtual Sequence<T>* Build() override {
            Sequence<T>* res = seq;
            seq = nullptr;
            return res;
        }
    };

    virtual ISequenceBuilder<T>* CreateBuilder() const override { return new Builder(); }
    virtual Sequence<T>* create_empty() const override { return new ImmutableArraySequence<T>(); }
    virtual Sequence<T>* clone() const override { return new ImmutableArraySequence<T>(*this); }

    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(T* items, int count) : ArraySequence<T>(items, count) {}
    ImmutableArraySequence(const LinkedList<T>& list) : ArraySequence<T>(list) {}
    ImmutableArraySequence(const ArraySequence<T>& seq) : ArraySequence<T>(seq) {}
    virtual ~ImmutableArraySequence() {}
};

#endif // IMMUTABLE_ARRAY_SEQUENCE_HPP