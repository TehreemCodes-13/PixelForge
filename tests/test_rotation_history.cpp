
#include <iostream>
#include "../src/HistoryManager.h"

using namespace std;

bool samePixel(Pixel a, Pixel b) {
    return a.red == b.red &&
           a.green == b.green &&
           a.blue == b.blue;
}

int main() {
    Image img(2, 3);

    int value = 10;

    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            Pixel p = {
                (unsigned char)value,
                (unsigned char)(value + 1),
                (unsigned char)(value + 2)
            };

            img.setPixel(x, y, p);
            value += 10;
        }
    }

    Image original = img;

    HistoryRecord record =
        createHistoryRecord(img, HistoryRecord::ROTATE_90);

    img.rotate90();

    applyHistoryOperation(img, record, true);

    if (img.getWidth() != original.getWidth() ||
        img.getHeight() != original.getHeight()) {
        cout << "FAIL: Dimensions were not restored.\n";
        return 1;
    }

    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            if (!samePixel(img.getPixel(x, y),
                           original.getPixel(x, y))) {
                cout << "FAIL: Original pixels were not restored.\n";
                return 1;
            }
        }
    }

    cout << "PASS: Rotation undo restored the original image.\n";
    return 0;
}
