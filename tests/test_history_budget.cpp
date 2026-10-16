
#include <iostream>
#include "../src/HistoryBudget.h"
using namespace std;

int main() {
    HistoryBudget history(500);
    HistoryRecord first, second, retrieved;

    first.estimatedMemory = 200;
    second.estimatedMemory = 200;
    first.parameter = 10;
    second.parameter = 20;

    history.addRecord(first);
    history.addRecord(second);

    if (!history.getLatestRecord(retrieved) || retrieved.parameter != 20) {
        cout << "FAIL: Latest record retrieval.\n";
        return 1;
    }

    if (!history.removeLatestRecord() ||
        history.size() != 1 ||
        history.getMemoryUsed() != 200) {
        cout << "FAIL: Latest record removal.\n";
        return 1;
    }

    HistoryBudget smallBudget(300);
    HistoryRecord oldRecord, newRecord, remaining;

    oldRecord.estimatedMemory = 200;
    oldRecord.parameter = 1;
    newRecord.estimatedMemory = 200;
    newRecord.parameter = 2;

    smallBudget.addRecord(oldRecord);
    smallBudget.addRecord(newRecord);

    if (smallBudget.size() != 1 ||
        smallBudget.getMemoryUsed() != 200 ||
        smallBudget.getDiscardedCount() != 1 ||
        !smallBudget.getLatestRecord(remaining) ||
        remaining.parameter != 2) {
        cout << "FAIL: Oldest record was not evicted correctly.\n";
        return 1;
    }

    cout << "PASS: Record retrieval and removal.\n";
    cout << "PASS: Memory budget evicts the oldest record.\n";
    return 0;
}
