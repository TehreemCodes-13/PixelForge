
#include <iostream>
#include "../src/HistoryController.h"

using namespace std;

int main() {
    HistoryController history(1024 * 1024);

    Image img(2, 2);
    Pixel red = {255, 0, 0};
    Pixel green = {0, 255, 0};

    img.setPixel(0, 0, red);
    img.setPixel(1, 0, green);

    HistoryRecord record;
    record.operationType = HistoryRecord::FLIP_HORIZONTAL;

    history.recordOperation(record);
    img.flipHorizontal();

    if (!history.canUndo() || !history.undo(img)) {
        cout << "FAIL: Undo did not run.\n";
        return 1;
    }

    if (img.getPixel(0, 0).red != red.red ||
        img.getPixel(1, 0).green != green.green) {
        cout << "FAIL: Undo did not restore original pixels.\n";
        return 1;
    }

    cout << "PASS: Controller undo restored original pixels.\n";
    return 0;
}
