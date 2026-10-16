
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
    Image original(3, 3);

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            Pixel p = {
                (unsigned char)(x * 50 + 10),
                (unsigned char)(y * 50 + 20),
                (unsigned char)(x * 20 + y * 30 + 5)
            };

            original.setPixel(x, y, p);
        }
    }

    Image gray = original;

    HistoryRecord grayRecord =
        createHistoryRecord(gray, HistoryRecord::GRAYSCALE);

    gray.convertToGrayscale();
    applyHistoryOperation(gray, grayRecord, true);

    if (!sameImage(gray, original)) {
        cout << "FAIL: Grayscale undo failed.\n";
        return 1;
    }

    Image blurred = original;

    HistoryRecord blurRecord =
        createHistoryRecord(blurred, HistoryRecord::BLUR, 1);

    blurred.blur(1);
    applyHistoryOperation(blurred, blurRecord, true);

    if (!sameImage(blurred, original)) {
        cout << "FAIL: Blur undo failed.\n";
        return 1;
    }

    cout << "PASS: Grayscale and blur undo restored original pixels.\n";
    return 0;
}
