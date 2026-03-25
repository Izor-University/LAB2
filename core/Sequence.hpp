#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "exceptions.hpp"
#include "Option.hpp"
#include "IEnumerable.hpp"
#include "ICollection.hpp"
#include "ISequenceBuilder.hpp"
#include <ostream>

template <class T>
class Sequence : public IEnumerable<T>, public ICollection<T> {
protected:
    // Общая логика для метода Slice
    void NormalizeSliceParameters(int length, int& index, int& count) const;

public:
    virtual ~Sequence() {}

    virtual ISequenceBuilder<T>* CreateBuilder() const = 0;

    // Ковариантные переопределения методов ICollection
    virtual Sequence<T>* create_empty() const override = 0;
    virtual Sequence<T>* clone() const override = 0;

    // Декомпозиция
    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;

    // Переопределение методов из ICollection
    virtual const T& Get(int index) const override = 0;
    virtual int GetLength() const override = 0; // Теперь это override из ICollection

    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;

    // Операции
    virtual Sequence<T>* Append(const T& item) = 0;
    virtual Sequence<T>* Prepend(const T& item) = 0;
    virtual Sequence<T>* InsertAt(const T& item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* list) const = 0;

    virtual const T& operator[](int index) const = 0;

    // Try-семантика
    virtual Option<T> TryGetFirst() const;
    virtual Option<T> TryGetLast() const;
    virtual Option<T> TryGet(int index) const;

    // Map-Reduce
    virtual Sequence<T>* map(T (*mapper)(const T& element)) const = 0;
    virtual Sequence<T>* where(bool (*predicate)(const T& element)) const = 0;
    virtual T reduce(T (*reduce_func)(const T& first_element, const T& second_element), const T& start_element) const = 0;

    // Срез
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* insertSeq = nullptr) = 0;
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq);

#include "Sequence.tpp"
#endif // SEQUENCE_HPP