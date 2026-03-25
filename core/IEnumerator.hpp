#ifndef IENUMERATOR_HPP
#define IENUMERATOR_HPP

template <class T>
class IEnumerator {
public:
    virtual ~IEnumerator() {}

    // Переход к следующему элементу. Возвращает false, если достигнут конец.
    virtual bool MoveNext() = 0;

    // Получение текущего элемента
    virtual const T& GetCurrent() const = 0;

    // Сброс итератора в начало
    virtual void Reset() = 0;
};

#endif // IENUMERATOR_HPP