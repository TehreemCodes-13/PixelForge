
#include <iostream>
#include "../src/HistoryManager.h"
#include "../src/HistoryBudget.h"

using namespace std;

int main() {
    Image img(2, 2);

    Pixel red = {255, 0, 0};
    Pixel green = {0, 255, 0};
    Pixel blue = {0, 0, 255};
    Pixel white = {255, 255, 255};

    img.setPixel(0, 0, red);
    img.setPixel(1, 0, green);
    img.setPixel(0, 1, blue);
    img.setPixel(1, 1, white);

    HistoryRecord record;
    record.operationType = HistoryRecord::FLIP_HORIZONTAL;

    applyHistoryOperation(img, record, false);
    applyHistoryOperation(img, record, true);

    if (img.getPixel(0, 0).red != red.red ||
        img.getPixel(0, 0).green != red.green ||
        img.getPixel(0, 0).blue != red.blue ||
        img.getPixel(1, 0).green != green.green ||
        img.getPixel(0, 1).blue != blue.blue ||
        img.getPixel(1, 1).red != white.red) {
        cout << "FAIL: Flip undo did not restore the original image.\n";
        return 1;
    }

    HistoryBudget history(1024 * 1024);
    record.estimatedMemory = sizeof(HistoryRecord);
    history.addRecord(record);

    HistoryRecord retrieved;
    if (!history.getLatestRecord(retrieved) ||
        retrieved.operationType != HistoryRecord::FLIP_HORIZONTAL) {
        cout << "FAIL: Could not retrieve the stored history record.\n";
        return 1;
    }

    cout << "PASS: Flip undo and history record storage.\n";
    return 0;
}
