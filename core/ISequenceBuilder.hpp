#ifndef ISEQUENCE_BUILDER_HPP
#define ISEQUENCE_BUILDER_HPP

// Предварительное объявление, чтобы избежать циклических зависимостей
template <class T>
class Sequence;

template <class T>
class ISequenceBuilder {
public:
    virtual ~ISequenceBuilder() {}

    // Добавляет элемент и возвращает указатель на самого себя
    // (позволяет делать цепочки вызовов: builder->Append(1)->Append(2))
    virtual ISequenceBuilder<T>* Append(const T& item) = 0;

    // Создает и возвращает готовую последовательность нужного типа
    virtual Sequence<T>* Build() = 0;
};

#endif // ISEQUENCE_BUILDER_HPP