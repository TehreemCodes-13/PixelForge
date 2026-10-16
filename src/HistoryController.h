
#pragma once
#include "HistoryBudget.h"

class HistoryController {
private:
    HistoryBudget undoHistory;
    HistoryBudget redoHistory;

public:
    explicit HistoryController(long long budgetBytes)
        : undoHistory(budgetBytes), redoHistory(budgetBytes) {
    }

    void recordOperation(const HistoryRecord& record) {
        undoHistory.addRecord(record);
        redoHistory.clear();
    }

    bool canUndo() const {
        return !undoHistory.isEmpty();
    }

    bool canRedo() const {
        return !redoHistory.isEmpty();
    }

    bool undo(Image& img) {
        HistoryRecord record;

        if (!undoHistory.getLatestRecord(record))
            return false;

        applyHistoryOperation(img, record, true);
        undoHistory.removeLatestRecord();
        redoHistory.addRecord(record);
        return true;
    }

    bool redo(Image& img) {
        HistoryRecord record;

        if (!redoHistory.getLatestRecord(record))
            return false;

        applyHistoryOperation(img, record, false);
        redoHistory.removeLatestRecord();
        undoHistory.addRecord(record);
        return true;
    }

    long long getUndoMemoryUsed() const {
        return undoHistory.getMemoryUsed();
    }

    long long getRedoMemoryUsed() const {
        return redoHistory.getMemoryUsed();
    }
};
