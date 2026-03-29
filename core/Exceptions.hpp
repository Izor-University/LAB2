#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

// Базовый класс для всех исключений нашей библиотеки
class Exception {
protected:
    const char* message;

public:
    explicit Exception(const char* msg) : message(msg) {}
    virtual ~Exception() {}

    virtual const char* GetMessage() const {
        return message;
    }
};

// Исключение, требуемое по ТЗ для неверных индексов
class IndexOutOfRange : public Exception {
public:
    explicit IndexOutOfRange(const char* msg = "Index out of range")
        : Exception(msg) {}
};

// Можно сразу добавить исключение для пустого списка/массива (пригодится для GetFirst/GetLast)
class EmptyCollectionError : public Exception {
public:
    explicit EmptyCollectionError(const char* msg = "Collection is empty")
        : Exception(msg) {}
};

#endif // EXCEPTIONS_HPP