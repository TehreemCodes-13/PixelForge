
#include <iostream>
#include "../src/HistoryManager.h"

using namespace std;

int main() {
    Image img(2, 1);

    Pixel p1 = {240, 100, 10};
    Pixel p2 = {20, 50, 100};

    img.setPixel(0, 0, p1);
    img.setPixel(1, 0, p2);

    HistoryRecord record =
        createHistoryRecord(img, HistoryRecord::BRIGHTNESS, 30);

    img.adjustBrightness(30);
    applyHistoryOperation(img, record, true);

    Pixel a = img.getPixel(0, 0);
    Pixel b = img.getPixel(1, 0);

    if (a.red == 240 && a.green == 100 && a.blue == 10 &&
        b.red == 20 && b.green == 50 && b.blue == 100) {
        cout << "PASS: Brightness undo restored original pixels.\n";
        return 0;
    }

    cout << "FAIL: Brightness undo did not restore original pixels.\n";
    return 1;
}
