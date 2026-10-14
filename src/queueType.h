#pragma once
#include <stdexcept>

template <class Type>
class queueType {
private:
    Type* list;
    int maxQueueSize;
    int queueFront;
    int queueRear;
    int count;

public:
    queueType(int queueSize = 100) {
        if (queueSize <= 0)
            throw std::invalid_argument("Invalid queue capacity");

        maxQueueSize = queueSize;
        list = new Type[maxQueueSize];
        queueFront = 0;
        queueRear = maxQueueSize - 1;
        count = 0;
    }

    ~queueType() {
        delete[] list;
    }

    queueType(const queueType& other) {
        maxQueueSize = other.maxQueueSize;
        queueFront = other.queueFront;
        queueRear = other.queueRear;
        count = other.count;
        list = new Type[maxQueueSize];

        for (int i = 0; i < maxQueueSize; i++)
            list[i] = other.list[i];
    }

    queueType& operator=(const queueType& other) {
        if (this == &other)
            return *this;

        Type* newList = new Type[other.maxQueueSize];

        try {
            for (int i = 0; i < other.maxQueueSize; i++)
                newList[i] = other.list[i];
        }
        catch (...) {
            delete[] newList;
            throw;
        }

        delete[] list;
        list = newList;
        maxQueueSize = other.maxQueueSize;
        queueFront = other.queueFront;
        queueRear = other.queueRear;
        count = other.count;

        return *this;
    }

    void initializeQueue() {
        queueFront = 0;
        queueRear = maxQueueSize - 1;
        count = 0;
    }

    bool isEmptyQueue() const {
        return count == 0;
    }

    bool isFullQueue() const {
        return count == maxQueueSize;
    }

    Type front() const {
        if (isEmptyQueue())
            throw std::underflow_error("Queue is empty");

        return list[queueFront];
    }

    Type back() const {
        if (isEmptyQueue())
            throw std::underflow_error("Queue is empty");

        return list[queueRear];
    }
    
    Type getAt(int position) const {
        if (position < 0 || position >= count)
            throw std::out_of_range("Invalid queue position");

        int index = (queueFront + position) % maxQueueSize;
        return list[index];
    }


    void addQueue(const Type& queueElement) {
        if (isFullQueue())
            throw std::overflow_error("Queue is full");

        queueRear = (queueRear + 1) % maxQueueSize;
        list[queueRear] = queueElement;
        count++;
    }

    void deleteQueue() {
        if (isEmptyQueue())
            throw std::underflow_error("Queue is empty");

        queueFront = (queueFront + 1) % maxQueueSize;
        count--;
    }

    int size() const {
        return count;
    }

    int capacity() const {
        return maxQueueSize;
    }
};
