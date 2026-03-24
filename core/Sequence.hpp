#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "exceptions.hpp"
#include "Option.hpp"
#include "Enumerator.hpp"
#include "ISequenceBuilder.hpp"

template <class T>
class Sequence : public IEnumerable<T> {
public:
    virtual ~Sequence() {
    }

    // --- ФАБРИЧНЫЙ МЕТОД БИЛДЕРА ---
    virtual ISequenceBuilder<T>* CreateBuilder() const = 0;

    // Декомпозиция
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(int index) const = 0;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;
    virtual int GetLength() const = 0;

    // Операции
    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* list) const = 0;

    // Бонусные задания: Перегрузка операторов и Try-семантика
    virtual T operator[](int index) const = 0;
    virtual Option<T> TryGetFirst() const = 0;
    virtual Option<T> TryGetLast() const = 0;
    virtual Option<T> TryGet(int index) const = 0;

    // Бонус: Map-Reduce
    virtual Sequence<T>* map(T (*mapper)(const T& element)) const = 0;
    virtual Sequence<T>* where(bool (*predicate)(const T& element)) const = 0;
    virtual T reduce(T (*reduce_func)(const T& first_element, const T& second_element), const T& start_element) const = 0;

    // Бонус: Срез
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* insertSeq = nullptr) = 0;
};

#include "Sequence.tpp"
#endif // SEQUENCE_HPP