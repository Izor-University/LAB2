#ifndef SEQUENCE_BUILDERS_HPP
#define SEQUENCE_BUILDERS_HPP

#include "ISequenceBuilder.hpp"
#include "MutableArraySequence.hpp"
#include "MutableListSequence.hpp"

// Строитель для массивов
template <class T, class TargetSequenceType>
class ArraySequenceBuilder : public ISequenceBuilder<T> {
private:
    // Быстрый изменяемый буфер для накопления элементов
    MutableArraySequence<T>* temporaryBuffer;

public:
    ArraySequenceBuilder() {
        temporaryBuffer = new MutableArraySequence<T>();
    }

    virtual ~ArraySequenceBuilder() {
        delete temporaryBuffer;
    }

    virtual ISequenceBuilder<T>* Append(T item) override {
        temporaryBuffer->Append(item);
        return this;
    }

    virtual Sequence<T>* Build() override {
        // Создаем финальный объект нужного типа (TargetSequenceType),
        // копируя данные из временного буфера.
        Sequence<T>* resultSequence = new TargetSequenceType(*temporaryBuffer);
        return resultSequence;
    }
};

// Строитель для связных списков
template <class T, class TargetSequenceType>
class ListSequenceBuilder : public ISequenceBuilder<T> {
private:
    // Быстрый изменяемый буфер для накопления элементов
    MutableListSequence<T>* temporaryBuffer;

public:
    ListSequenceBuilder() {
        temporaryBuffer = new MutableListSequence<T>();
    }

    virtual ~ListSequenceBuilder() {
        delete temporaryBuffer;
    }

    virtual ISequenceBuilder<T>* Append(T item) override {
        temporaryBuffer->Append(item);
        return this;
    }

    virtual Sequence<T>* Build() override {
        Sequence<T>* resultSequence = new TargetSequenceType(*temporaryBuffer);
        return resultSequence;
    }
};

#endif // SEQUENCE_BUILDERS_HPP