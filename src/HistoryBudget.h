
#pragma once
#include "HistoryManager.h"
#include <stdexcept>

class HistoryBudget {
private:
    HistoryRecord* records;
    int count;
    int capacity;
    long long memoryUsed;
    long long memoryLimit;
    int discardedCount;

    void grow() {
        int newCapacity = capacity == 0 ? 8 : capacity * 2;
        HistoryRecord* newRecords = new HistoryRecord[newCapacity];

        for (int i = 0; i < count; i++)
            newRecords[i] = records[i];

        delete[] records;
        records = newRecords;
        capacity = newCapacity;
    }

    void removeOldest() {
        if (count == 0)
            return;

        memoryUsed -= records[0].estimatedMemory;

        for (int i = 1; i < count; i++)
            records[i - 1] = records[i];

        count--;
        discardedCount++;
    }

public:
    explicit HistoryBudget(long long budgetBytes) {
        if (budgetBytes < 0)
            throw std::invalid_argument("History budget cannot be negative");

        records = nullptr;
        count = 0;
        capacity = 0;
        memoryUsed = 0;
        memoryLimit = budgetBytes;
        discardedCount = 0;
    }

    ~HistoryBudget() {
        delete[] records;
    }

    HistoryBudget(const HistoryBudget&) = delete;
    HistoryBudget& operator=(const HistoryBudget&) = delete;

    bool canCoalesceBrightness(const HistoryRecord& record) const {
        return count > 0 &&
            records[count - 1].operationType == HistoryRecord::BRIGHTNESS &&
            record.operationType == HistoryRecord::BRIGHTNESS;
    }

    void addRecord(const HistoryRecord& record) {
        if (canCoalesceBrightness(record)) {
            HistoryRecord merged(records[count - 1]);

            // The merge helper must preserve each adjustment and its
            // clipped-pixel data in operation order.
            merged.mergeBrightnessRecord(record);

            long long newMemory = merged.estimatedMemory;
            long long oldMemory = records[count - 1].estimatedMemory;

            if (newMemory <= memoryLimit) {
                while (count > 1 &&
                       memoryUsed - oldMemory + newMemory > memoryLimit) {
                    removeOldest();
                    oldMemory = records[count - 1].estimatedMemory;
                }

                if (memoryUsed - oldMemory + newMemory <= memoryLimit) {
                    records[count - 1] = merged;
                    memoryUsed = memoryUsed - oldMemory + newMemory;
                    return;
                }
            }
        }

        long long recordMemory = record.estimatedMemory;

        if (recordMemory > memoryLimit) {
            discardedCount++;
            return;
        }

        while (count > 0 && memoryUsed + recordMemory > memoryLimit)
            removeOldest();

        if (count == capacity)
            grow();

        records[count] = record;
        count++;
        memoryUsed += recordMemory;
    }

    bool getLatestRecord(HistoryRecord& record) const {
        if (count == 0)
            return false;

        record = records[count - 1];
        return true;
    }

    bool removeLatestRecord() {
        if (count == 0)
            return false;

        memoryUsed -= records[count - 1].estimatedMemory;
        count--;
        return true;
    }

    int size() const {
        return count;
    }

    long long getMemoryUsed() const {
        return memoryUsed;
    }

    long long getMemoryLimit() const {
        return memoryLimit;
    }

    int getDiscardedCount() const {
        return discardedCount;
    }

    bool isEmpty() const {
        return count == 0;
    }

    void clear() {
        count = 0;
        memoryUsed = 0;
    }
};
