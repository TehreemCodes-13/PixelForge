
#include <iostream>
#include "../src/HistoryManager.h"
#include "../src/HistoryBudget.h"
using namespace std;

int main() {
    Image img(2, 1);
    Pixel p1 = {100, 120, 140};
    Pixel p2 = {250, 10, 20};
    img.setPixel(0, 0, p1);
    img.setPixel(1, 0, p2);

    Pixel original1 = img.getPixel(0, 0);
    Pixel original2 = img.getPixel(1, 0);

    HistoryBudget history(1024 * 1024);

    HistoryRecord first =
        createHistoryRecord(img, HistoryRecord::BRIGHTNESS, 200);
    img.adjustBrightness(200);
    history.addRecord(first);

    HistoryRecord second =
        createHistoryRecord(img, HistoryRecord::BRIGHTNESS, -100);
    img.adjustBrightness(-100);
    history.addRecord(second);

    if (history.size() != 1) {
        cout << "FAIL: Brightness records were not merged.\n";
        return 1;
    }

    HistoryRecord merged;
    if (!history.getLatestRecord(merged) ||
        merged.adjustmentCount != 2) {
        cout << "FAIL: Merged record did not preserve both adjustments.\n";
        return 1;
    }

    // Test undo first.
    applyHistoryOperation(img, merged, true);

    Pixel result1 = img.getPixel(0, 0);
    Pixel result2 = img.getPixel(1, 0);

    if (result1.red != original1.red ||
        result1.green != original1.green ||
        result1.blue != original1.blue ||
        result2.red != original2.red ||
        result2.green != original2.green ||
        result2.blue != original2.blue) {
        cout << "FAIL: Undo did not restore the exact original pixels.\n";
        return 1;
    }

    cout << "PASS: Brightness records merged into one record.\n";
    cout << "PASS: Clipping-aware undo restored original pixels.\n";

    // Now test redo separately.
    applyHistoryOperation(img, merged, false);

    Pixel redone1 = img.getPixel(0, 0);
    Pixel redone2 = img.getPixel(1, 0);

    if (redone1.red != 155 ||
        redone1.green != 155 ||
        redone1.blue != 155 ||
        redone2.red != 155 ||
        redone2.green != 110 ||
        redone2.blue != 120) {
        cout << "FAIL: Redo did not replay brightness operations correctly.\n";
        return 1;
    }

    cout << "PASS: Redo replayed merged brightness operations.\n";
    return 0;
}
