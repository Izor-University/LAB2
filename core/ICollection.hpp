#ifndef ICOLLECTION_HPP
#define ICOLLECTION_HPP

template <class T>
class ICollection {
public:
    virtual ~ICollection() {}

    // Основные методы коллекции
    virtual const T& Get(int index) const = 0;
    virtual int GetLength() const = 0;

    // Виртуальный конструктор пустого объекта
    virtual ICollection<T>* create_empty() const = 0;

    // Виртуальный копирующий конструктор (Prototype)
    virtual ICollection<T>* clone() const = 0;
};

#endif // ICOLLECTION_HPP