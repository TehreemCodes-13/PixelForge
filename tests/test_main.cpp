
#include <iostream>
#include "../src/Image.h"
using namespace std;

int main() {
    Image img;

    if (!img.loadImage("images/character.png")) {
        cout << "Image Loading: FAIL" << endl;
        return 1;
    }

    cout << "Image Loading: PASS" << endl;
    cout << "Width: " << img.getWidth() << endl;
    cout << "Height: " << img.getHeight() << endl;

    if (img.saveImage("images/output.png")) {
        cout << "Image Saving: PASS" << endl;
    } else {
        cout << "Image Saving: FAIL" << endl;
        return 1;
    }

    Image cropTest(4, 3);

    int value = 1;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 4; x++) {
            Pixel p = {(unsigned char)value++, 0, 0};
            cropTest.setPixel(x, y, p);
        }
    }

    cropTest.crop(1, 1, 2, 2);

    if (cropTest.getWidth() == 2 &&
        cropTest.getHeight() == 2 &&
        cropTest.getPixel(0, 0).red == 6 &&
        cropTest.getPixel(1, 0).red == 7 &&
        cropTest.getPixel(0, 1).red == 10 &&
        cropTest.getPixel(1, 1).red == 11) {
        cout << "Crop: PASS" << endl;
    } else {
        cout << "Crop: FAIL" << endl;
        return 1;
    }

    cout << "Crop test completed successfully." << endl;

    if (!img.loadImage("images/character.png")) {
        cout << "Image Reloading: FAIL" << endl;
        return 1;
    }

    img.resize(200, 200);

    if (img.getWidth() == 200 && img.getHeight() == 200) {
        cout << "Resize Dimensions: PASS" << endl;
    } else {
        cout << "Resize Dimensions: FAIL" << endl;
        return 1;
    }

    if (img.saveImage("images/resized_test.png")) {
        cout << "Resize Save: PASS" << endl;
    } else {
        cout << "Resize Save: FAIL" << endl;
        return 1;
    }
    Image blurTest(3, 3);

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            Pixel p = {0, 0, 0};
            blurTest.setPixel(x, y, p);
        }
    }

    Pixel center = {255, 150, 90};
    blurTest.setPixel(1, 1, center);

    blurTest.blur(1);

    Pixel blurredCenter = blurTest.getPixel(1, 1);

    if (blurredCenter.red == 28 &&
        blurredCenter.green == 16 &&
        blurredCenter.blue == 10) {
        cout << "Blur Pixel Values: PASS" << endl;
    } else {
        cout << "Blur Pixel Values: FAIL" << endl;
        cout << "Actual RGB: "
             << (int)blurredCenter.red << ", "
             << (int)blurredCenter.green << ", "
             << (int)blurredCenter.blue << endl;
        return 1;
    }
    cout << "All tests passed!" << endl;
    return 0;
}
