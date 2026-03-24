// Этот файл подключается в конце LinkedList.hpp

template <class T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), size(0) {}

template <class T>
LinkedList<T>::LinkedList(T* items, int count) : LinkedList() {
    if (count < 0) {
        throw IndexOutOfRange("Error: Count cannot be negative!");
    }
    for (int i = 0; i < count; ++i) {
        Append(items[i]);
    }
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T>& list) : LinkedList() {
    Node* current = list.head;
    while (current != nullptr) {
        Append(current->data);
        current = current->next;
    }
}

template <class T>
LinkedList<T>::~LinkedList() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

template <class T>
const T& LinkedList<T>::GetFirst() const {
    if (size == 0) {
        throw IndexOutOfRange("Error: List is empty in GetFirst()!");
    }
    return head->data;
}

template <class T>
const T& LinkedList<T>::GetLast() const {
    if (size == 0) {
        throw IndexOutOfRange("Error: List is empty in GetLast()!");
    }
    return tail->data;
}

template <class T>
const T& LinkedList<T>::Get(int index) const {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Get()!");
    }
    Node* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return current->data;
}

template <class T>
LinkedList<T>* LinkedList<T>::GetSubList(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= size || startIndex > endIndex) {
        throw IndexOutOfRange("Error: Invalid indices in GetSubList()!");
    }
    LinkedList<T>* subList = new LinkedList<T>();
    Node* current = head;
    for (int i = 0; i <= endIndex; ++i) {
        if (i >= startIndex) {
            subList->Append(current->data);
        }
        current = current->next;
    }
    return subList;
}

template <class T>
int LinkedList<T>::GetLength() const {
    return size;
}

template <class T>
void LinkedList<T>::Append(const T& item) {
    Node* newNode = new Node(item);
    if (size == 0) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    size++;
}

template <class T>
void LinkedList<T>::Prepend(const T& item) {
    Node* newNode = new Node(item, head);
    head = newNode;
    if (size == 0) {
        tail = newNode;
    }
    size++;
}

template <class T>
void LinkedList<T>::InsertAt(const T& item, int index) {
    if (index < 0 || index > size) {
        throw IndexOutOfRange("Error: Index out of range in InsertAt()!");
    }

    if (index == 0) {
        Prepend(item);
    } else if (index == size) {
        Append(item);
    } else {
        Node* current = head;
        for (int i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        Node* newNode = new Node(item, current->next);
        current->next = newNode;
        size++;
    }
}

template <class T>
LinkedList<T>* LinkedList<T>::Concat(LinkedList<T>* list) const {
    LinkedList<T>* newList = new LinkedList<T>();
    Node* current = this->head;
    while (current != nullptr) {
        newList->Append(current->data);
        current = current->next;
    }

    if (list != nullptr) {
        current = list->head;
        while (current != nullptr) {
            newList->Append(current->data);
            current = current->next;
        }
    }

    return newList;
}