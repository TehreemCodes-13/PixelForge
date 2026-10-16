#pragma once
#include <stdexcept>

template <class Type>
class stackType {
private:
    struct Node {
        Type data;
        Node* next;

        Node(const Type& value) {
            data = value;
            next = nullptr;
        }
    };

    Node* stackTop;
    int count;

    void copyFrom(const stackType<Type>& other) {
        Node* current = other.stackTop;
        Node* last = nullptr;

        try {
            while (current != nullptr) {
                Node* newNode = new Node(current->data);

                if (stackTop == nullptr)
                    stackTop = newNode;
                else
                    last->next = newNode;

                last = newNode;
                current = current->next;
                count++;
            }
        }
        catch (...) {
            initializeStack();
            throw;
        }
    }

public:
    stackType(int stackSize = 100) {
        stackTop = nullptr;
        count = 0;
    }

    ~stackType() {
        initializeStack();
    }

    stackType(const stackType<Type>& other) {
        stackTop = nullptr;
        count = 0;
        copyFrom(other);
    }

    
    stackType<Type>& operator=(const stackType<Type>& other) {
        if (this != &other) {
            initializeStack();
            copyFrom(other);
        }
        return *this;
    }


    void initializeStack() {
        while (stackTop != nullptr) {
            Node* temp = stackTop;
            stackTop = stackTop->next;
            delete temp;
        }

        count = 0;
    }

    bool isEmptyStack() const {
        return stackTop == nullptr;
    }

    bool isFullStack() const {
        return false;
    }

    void push(const Type& newItem) {
        Node* newNode = new Node(newItem);
        newNode->next = stackTop;
        stackTop = newNode;
        count++;
    }

    Type top() const {
        if (isEmptyStack())
            throw std::underflow_error("Stack is empty");

        return stackTop->data;
    }

    void pop() {
        if (isEmptyStack())
            throw std::underflow_error("Stack is empty");

        Node* temp = stackTop;
        stackTop = stackTop->next;
        delete temp;
        count--;
    }

    void removeBottom() {
        if (isEmptyStack())
            throw std::underflow_error("Stack is empty");

        if (stackTop->next == nullptr) {
            delete stackTop;
            stackTop = nullptr;
            count--;
            return;
        }

        Node* current = stackTop;

        while (current->next->next != nullptr)
            current = current->next;

        delete current->next;
        current->next = nullptr;
        count--;
    }

    int size() const {
        return count;
    }
};
