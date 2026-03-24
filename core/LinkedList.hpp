#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "exceptions.hpp"

template <class T>
class LinkedList {
private:
    // Внутренняя структура узла списка
    struct Node {
        T data;
        Node* next;
        Node(T data, Node* next = nullptr) : data(data), next(next) {}
    };

    Node* head; // Указатель на начало списка
    Node* tail; // Указатель на конец списка (оптимизирует Append и GetLast)
    int size;   // Текущий размер списка

public:
    // Создание объекта [cite: 100]
    LinkedList(T* items, int count);
    LinkedList();
    LinkedList(const LinkedList<T>& list);

    // Деструктор (обязателен для очистки памяти)
    ~LinkedList();

    // Декомпозиция
    T GetFirst() const;
    T GetLast() const;
    T Get(int index) const;
    LinkedList<T>* GetSubList(int startIndex, int endIndex) const;
    int GetLength() const;

    // Операции
    void Append(T item);
    void Prepend(T item);
    void InsertAt(T item, int index);
    LinkedList<T>* Concat(LinkedList<T>* list) const;
};

// Подключение реализации шаблонных методов
#include "LinkedList.tpp"

#endif // LINKED_LIST_HPP