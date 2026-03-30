#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "Exceptions.hpp"

template <class T>
class LinkedList {
private:
    // Внутренняя структура узла теперь двусвязная (prev и next)
    struct Node {
        T data;
        Node* next;
        Node* prev;

        explicit Node(const T& data) : data(data), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

    // Вспомогательный метод для быстрой очистки памяти (используется в = и ~)
    void Clear();

public:
    // Конструкторы
    LinkedList();
    LinkedList(T* items, int count);

    // Копирующий конструктор
    LinkedList(const LinkedList<T>& other);

    // Move-конструктор (конструктор перемещения)
    LinkedList(LinkedList<T>&& other) noexcept;

    // Деструктор
    ~LinkedList();

    // Операторы присваивания
    LinkedList<T>& operator=(const LinkedList<T>& other); // Строгая гарантия исключений
    LinkedList<T>& operator=(LinkedList<T>&& other) noexcept; // Move-присваивание

    // Декомпозиция
    const T& GetFirst() const;
    const T& GetLast() const;
    const T& Get(int index) const;
    int GetLength() const;

    LinkedList<T>* GetSubList(int startIndex, int endIndex) const;

    // Операции
    void Append(const T& item);
    void Prepend(const T& item);
    void InsertAt(const T& item, int index);
    void RemoveAt(int index); // Новый метод удаления

    LinkedList<T>* Concat(LinkedList<T>* list) const;
};

// Подключение реализации шаблонных методов
#include "LinkedList.tpp"
#endif // LINKED_LIST_HPP