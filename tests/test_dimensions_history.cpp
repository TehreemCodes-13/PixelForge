#include <iostream>
#include "../src/HistoryManager.h"

using namespace std;

bool samePixel(Pixel a, Pixel b) {
    return a.red == b.red &&
           a.green == b.green &&
           a.blue == b.blue;
}

bool sameImage(const Image& a, const Image& b) {
    if (a.getWidth() != b.getWidth() ||
        a.getHeight() != b.getHeight())
        return false;

    for (int y = 0; y < a.getHeight(); y++) {
        for (int x = 0; x < a.getWidth(); x++) {
            if (!samePixel(a.getPixel(x, y), b.getPixel(x, y)))
                return false;
        }
    }

    return true;
}

int main() {
    Image original(4, 3);
    int value = 10;

    for (int y = 0; y < original.getHeight(); y++) {
        for (int x = 0; x < original.getWidth(); x++) {
            Pixel p = {
                (unsigned char)value,
                (unsigned char)(value + 1),
                (unsigned char)(value + 2)
            };

            original.setPixel(x, y, p);
            value += 10;
        }
    }

    // Crop undo and redo
    Image cropped = original;
    HistoryRecord cropRecord =
        createHistoryRecord(cropped, HistoryRecord::CROP);

    cropRecord.x = 1;
    cropRecord.y = 0;
    cropRecord.width = 2;
    cropRecord.height = 2;

    cropped.crop(1, 0, 2, 2);
    Image expectedCropped = cropped;

    applyHistoryOperation(cropped, cropRecord, true);

    if (!sameImage(cropped, original)) {
        cout << "FAIL: Crop undo did not restore the original image.\n";
        return 1;
    }

    applyHistoryOperation(cropped, cropRecord, false);

    if (!sameImage(cropped, expectedCropped)) {
        cout << "FAIL: Crop redo did not restore the cropped image.\n";
        return 1;
    }

    // Resize undo and redo
    Image resized = original;
    HistoryRecord resizeRecord =
        createHistoryRecord(resized, HistoryRecord::RESIZE);

    resizeRecord.width = 2;
    resizeRecord.height = 2;

    resized.resize(2, 2);
    Image expectedResized = resized;

    applyHistoryOperation(resized, resizeRecord, true);

    if (!sameImage(resized, original)) {
        cout << "FAIL: Resize undo did not restore the original image.\n";
        return 1;
    }

    applyHistoryOperation(resized, resizeRecord, false);

    if (!sameImage(resized, expectedResized)) {
        cout << "FAIL: Resize redo did not restore the resized image.\n";
        return 1;
    }

    cout << "PASS: Crop undo and redo.\n";
    cout << "PASS: Resize undo and redo.\n";
    return 0;
}