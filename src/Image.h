#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <new>
#include <cstring>
#include <cstddef>
#include <stdexcept>
#include "stackType.h"
#include "queueType.h"

struct Pixel {
    unsigned char red, green, blue;
};

class Image {
private:
    Pixel* pixels;
    int width;
    int height;

public:
    Image() {
        pixels = nullptr;
        width = 0;
        height = 0;
    }

    Image(int w, int h) {
        if (w <= 0 || h <= 0)
            throw std::invalid_argument("Invalid image dimensions");

        width = w;
        height = h;
        pixels = new Pixel[width * height]{};
    }

    Image(const Image& other) {
        width = other.width;
        height = other.height;
        pixels = nullptr;

        if (other.pixels != nullptr) {
            pixels = new Pixel[width * height];
            for (int i = 0; i < width * height; i++)
                pixels[i] = other.pixels[i];
        }
    }

    Image& operator=(const Image& other) {
        if (this == &other)
            return *this;

        Pixel* newPixels = nullptr;

        if (other.pixels != nullptr) {
            newPixels = new Pixel[other.width * other.height];

            for (int i = 0; i < other.width * other.height; i++)
                newPixels[i] = other.pixels[i];
        }

        delete[] pixels;
        pixels = newPixels;
        width = other.width;
        height = other.height;

        return *this;
    }

    ~Image() {
        delete[] pixels;
    }

    bool isLoaded() const {
        return pixels != nullptr;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    Pixel getPixel(int x, int y) const {
        if (pixels == nullptr || x < 0 || x >= width ||
            y < 0 || y >= height)
            throw std::out_of_range("Invalid pixel position");

        return pixels[y * width + x];
    }

    void setPixel(int x, int y, Pixel pixel) {
        if (pixels == nullptr || x < 0 || x >= width ||
            y < 0 || y >= height)
            throw std::out_of_range("Invalid pixel position");

        pixels[y * width + x] = pixel;
    }
    
    bool isSameColor(Pixel a, Pixel b) const {
        return a.red == b.red &&
            a.green == b.green &&
            a.blue == b.blue;
    }

    void bucketFillWithStack(int startX, int startY,
                            Pixel newColor,
                            stackType<int>& pixelStack) {
        if (pixels == nullptr)
            throw std::runtime_error("No image loaded");

        if (startX < 0 || startX >= width ||
            startY < 0 || startY >= height)
            throw std::out_of_range("Invalid fill position");

        Pixel oldColor = getPixel(startX, startY);

        if (isSameColor(oldColor, newColor))
            return;

        // Clear any leftover entries before starting.
        pixelStack.initializeStack();

        int startPosition = startY * width + startX;
        pixelStack.push(startPosition);
        setPixel(startX, startY, newColor);

        while (!pixelStack.isEmptyStack()) {
            int position = pixelStack.top();
            pixelStack.pop();

            int x = position % width;
            int y = position / width;

            // Up
            if (y > 0 && isSameColor(getPixel(x, y - 1), oldColor)) {
                setPixel(x, y - 1, newColor);
                pixelStack.push((y - 1) * width + x);
            }

            // Down
            if (y < height - 1 &&
                isSameColor(getPixel(x, y + 1), oldColor)) {
                setPixel(x, y + 1, newColor);
                pixelStack.push((y + 1) * width + x);
            }

            // Left
            if (x > 0 && isSameColor(getPixel(x - 1, y), oldColor)) {
                setPixel(x - 1, y, newColor);
                pixelStack.push(y * width + (x - 1));
            }

            // Right
            if (x < width - 1 &&
                isSameColor(getPixel(x + 1, y), oldColor)) {
                setPixel(x + 1, y, newColor);
                pixelStack.push(y * width + (x + 1));
            }
        }
    }

    void bucketFillWithQueue(int startX, int startY,
                            Pixel newColor,
                            queueType<int>& pixelQueue) {
        if (pixels == nullptr)
            throw std::runtime_error("No image loaded");

        if (startX < 0 || startX >= width ||
            startY < 0 || startY >= height)
            throw std::out_of_range("Invalid fill position");

        Pixel oldColor = getPixel(startX, startY);

        if (isSameColor(oldColor, newColor))
            return;

        pixelQueue.initializeQueue();

        int startPosition = startY * width + startX;
        pixelQueue.addQueue(startPosition);
        setPixel(startX, startY, newColor);

        while (!pixelQueue.isEmptyQueue()) {
            int position = pixelQueue.front();
            pixelQueue.deleteQueue();

            int x = position % width;
            int y = position / width;

            // Up
            if (y > 0 && isSameColor(getPixel(x, y - 1), oldColor)) {
                setPixel(x, y - 1, newColor);
                pixelQueue.addQueue((y - 1) * width + x);
            }

            // Down
            if (y < height - 1 &&
                isSameColor(getPixel(x, y + 1), oldColor)) {
                setPixel(x, y + 1, newColor);
                pixelQueue.addQueue((y + 1) * width + x);
            }

            // Left
            if (x > 0 && isSameColor(getPixel(x - 1, y), oldColor)) {
                setPixel(x - 1, y, newColor);
                pixelQueue.addQueue(y * width + (x - 1));
            }

            // Right
            if (x < width - 1 &&
                isSameColor(getPixel(x + 1, y), oldColor)) {
                setPixel(x + 1, y, newColor);
                pixelQueue.addQueue(y * width + (x + 1));
            }
        }
    }


    void clear() {
        delete[] pixels;
        pixels = nullptr;
        width = 0;
        height = 0;
    }

    void adjustBrightness(int amount) {
        if (pixels == nullptr)
            return;

        for (int i = 0; i < width * height; i++) {
            int r = pixels[i].red + amount;
            int g = pixels[i].green + amount;
            int b = pixels[i].blue + amount;

            if (r > 255) r = 255;
            if (r < 0) r = 0;
            if (g > 255) g = 255;
            if (g < 0) g = 0;
            if (b > 255) b = 255;
            if (b < 0) b = 0;

            pixels[i].red = r;
            pixels[i].green = g;
            pixels[i].blue = b;
        }
    }

    void convertToGrayscale() {
        if (pixels == nullptr)
            return;

        for (int i = 0; i < width * height; i++) {
            int gray = (pixels[i].red + pixels[i].green +
                        pixels[i].blue) / 3;

            pixels[i].red = gray;
            pixels[i].green = gray;
            pixels[i].blue = gray;
        }
    }
    void flipHorizontal() {
        if (pixels == nullptr)
            return;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width / 2; x++) {
                int oppositeX = width - 1 - x;

                Pixel temp = pixels[y * width + x];
                pixels[y * width + x] = pixels[y * width + oppositeX];
                pixels[y * width + oppositeX] = temp;
            }
        }
    }

    void flipVertical() {
        if (pixels == nullptr)
            return;

        for (int y = 0; y < height / 2; y++) {
            for (int x = 0; x < width; x++) {
                int oppositeY = height - 1 - y;

                Pixel temp = pixels[y * width + x];
                pixels[y * width + x] = pixels[oppositeY * width + x];
                pixels[oppositeY * width + x] = temp;
            }
        }
    }
    void rotate90() {
        if (pixels == nullptr)
            return;

        Image rotated(height, width);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                rotated.setPixel(height - 1 - y, x,
                                pixels[y * width + x]);
            }
        }

        *this = rotated;
    }

    void rotate180() {
        if (pixels == nullptr)
            return;

        for (int i = 0; i < (width * height) / 2; i++) {
            int opposite = width * height - 1 - i;

            Pixel temp = pixels[i];
            pixels[i] = pixels[opposite];
            pixels[opposite] = temp;
        }
    }

    void rotate270() {
        if (pixels == nullptr)
            return;

        Image rotated(height, width);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                rotated.setPixel(y, width - 1 - x,
                                pixels[y * width + x]);
            }
        }

        *this = rotated;
    }

    void blur(int radius) {
        if (pixels == nullptr)
            throw std::runtime_error("No image loaded");

        if (radius < 0)
            throw std::invalid_argument("Invalid blur radius");

        if (radius == 0)
            return;

        if (radius > width)
            radius = width;
        if (radius > height)
            radius = height;

        Image blurred(width, height);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int redSum = 0;
                int greenSum = 0;
                int blueSum = 0;
                int count = 0;

                int startX = x - radius;
                int endX = x + radius;
                int startY = y - radius;
                int endY = y + radius;

                if (startX < 0)
                    startX = 0;
                if (startY < 0)
                    startY = 0;
                if (endX >= width)
                    endX = width - 1;
                if (endY >= height)
                    endY = height - 1;

                for (int ny = startY; ny <= endY; ny++) {
                    for (int nx = startX; nx <= endX; nx++) {
                        Pixel p = pixels[ny * width + nx];

                        redSum += p.red;
                        greenSum += p.green;
                        blueSum += p.blue;
                        count++;
                    }
                }

                Pixel result;
                result.red = (unsigned char)(redSum / count);
                result.green = (unsigned char)(greenSum / count);
                result.blue = (unsigned char)(blueSum / count);

                blurred.setPixel(x, y, result);
            }
        }

        *this = blurred;
    }

    
    void resize(int newWidth, int newHeight) {
        if (pixels == nullptr)
            throw std::runtime_error("No image loaded");

        if (newWidth <= 0 || newHeight <= 0)
            throw std::invalid_argument("Invalid resize dimensions");

        Image resized(newWidth, newHeight);

        for (int y = 0; y < newHeight; y++) {
            for (int x = 0; x < newWidth; x++) {
                int sourceX = (int)((long long)x * width / newWidth);
                int sourceY = (int)((long long)y * height / newHeight);

                resized.setPixel(
                    x, y,
                    pixels[sourceY * width + sourceX]
                );
            }
        }

        *this = resized;
    }

    void crop(int startX, int startY, int cropWidth, int cropHeight) {
        if (pixels == nullptr)
            throw std::runtime_error("No image loaded");

        if (startX < 0 || startY < 0 ||
            cropWidth <= 0 || cropHeight <= 0 ||
            startX > width - cropWidth ||
            startY > height - cropHeight)
            throw std::out_of_range("Invalid crop region");

        Image cropped(cropWidth, cropHeight);

        for (int y = 0; y < cropHeight; y++) {
            for (int x = 0; x < cropWidth; x++) {
                cropped.setPixel(x, y,
                    pixels[(startY + y) * width + (startX + x)]);
            }
        }

        *this = cropped;
    }
    bool loadImage(const char* filename) {
        if (filename == nullptr)
            return false;

        int w = 0, h = 0, channels = 0;
        unsigned char* data = stbi_load(filename, &w, &h, &channels, 3);

        if (data == nullptr)
            return false;

        if (w <= 0 || h <= 0) {
            stbi_image_free(data);
            return false;
        }

        size_t count = static_cast<size_t>(w) * h;
        Pixel* newPixels = new (std::nothrow) Pixel[count];

        if (newPixels == nullptr) {
            stbi_image_free(data);
            return false;
        }

        for (size_t i = 0; i < count; i++) {
            newPixels[i].red = data[i * 3];
            newPixels[i].green = data[i * 3 + 1];
            newPixels[i].blue = data[i * 3 + 2];
        }

        stbi_image_free(data);

        delete[] pixels;
        pixels = newPixels;
        width = w;
        height = h;

        return true;
    }

    bool saveImage(const char* filename) const {
        if (filename == nullptr || pixels == nullptr || width <= 0 || height <= 0)
            return false;

        size_t count = static_cast<size_t>(width) * height;
        unsigned char* data = new (std::nothrow) unsigned char[count * 3];

        if (data == nullptr)
            return false;

        for (size_t i = 0; i < count; i++) {
            data[i * 3] = pixels[i].red;
            data[i * 3 + 1] = pixels[i].green;
            data[i * 3 + 2] = pixels[i].blue;
        }

        int result = 0;

        const char* extension = strrchr(filename, '.');

        if (extension != nullptr &&
            (strcmp(extension, ".jpg") == 0 ||
            strcmp(extension, ".jpeg") == 0 ||
            strcmp(extension, ".JPG") == 0 ||
            strcmp(extension, ".JPEG") == 0)) {
            result = stbi_write_jpg(filename, width, height, 3, data, 95);
        } else {
            result = stbi_write_png(filename, width, height, 3, data, width * 3);
        }

        delete[] data;
        return result != 0;
    }

};
