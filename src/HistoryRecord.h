
#pragma once
#include "Image.h"

struct HistoryRecord {
    static const int BRIGHTNESS = 1;
    static const int GRAYSCALE = 2;
    static const int FLIP_HORIZONTAL = 3;
    static const int FLIP_VERTICAL = 4;
    static const int ROTATE_90 = 5;
    static const int ROTATE_180 = 6;
    static const int ROTATE_270 = 7;
    static const int CROP = 8;
    static const int RESIZE = 9;
    static const int BLUR = 10;
    static const int BUCKET_FILL = 11;

    int operationType, parameter, secondaryParameter;
    int width, height, oldWidth, oldHeight, x, y, sequenceNumber;
    long long estimatedMemory;

    Pixel* oldPixels;
    int* pixelIndices;
    int pixelCount;
    int indexCount;

    int* brightnessAdjustments;
    int adjustmentCount;
    int* brightnessPixelOffsets;
    Pixel* brightnessOldPixels;
    int* brightnessPixelIndices;
    int brightnessPixelCount;

    HistoryRecord() {
        operationType = parameter = secondaryParameter = 0;
        width = height = oldWidth = oldHeight = x = y = sequenceNumber = 0;
        estimatedMemory = sizeof(HistoryRecord);
        oldPixels = nullptr;
        pixelIndices = nullptr;
        pixelCount = indexCount = 0;
        brightnessAdjustments = nullptr;
        adjustmentCount = 0;
        brightnessPixelOffsets = nullptr;
        brightnessOldPixels = nullptr;
        brightnessPixelIndices = nullptr;
        brightnessPixelCount = 0;
    }

    ~HistoryRecord() {
        delete[] oldPixels;
        delete[] pixelIndices;
        delete[] brightnessAdjustments;
        delete[] brightnessPixelOffsets;
        delete[] brightnessOldPixels;
        delete[] brightnessPixelIndices;
    }

    HistoryRecord(const HistoryRecord& other) : HistoryRecord() {
        operationType = other.operationType;
        parameter = other.parameter;
        secondaryParameter = other.secondaryParameter;
        width = other.width;
        height = other.height;
        oldWidth = other.oldWidth;
        oldHeight = other.oldHeight;
        x = other.x;
        y = other.y;
        sequenceNumber = other.sequenceNumber;

        if (other.pixelCount > 0 && other.oldPixels != nullptr)
            setOldPixels(other.oldPixels, other.pixelCount);

        if (other.indexCount > 0 && other.pixelIndices != nullptr)
            setPixelIndices(other.pixelIndices, other.indexCount);

        if (other.adjustmentCount > 0) {
            for (int i = 0; i < other.adjustmentCount; i++) {
                int start = other.brightnessPixelOffsets[i];
                int end = other.brightnessPixelOffsets[i + 1];
                addBrightnessStep(
                    other.brightnessAdjustments[i],
                    other.brightnessOldPixels + start,
                    other.brightnessPixelIndices + start,
                    end - start);
            }
        }
        estimatedMemory = other.estimatedMemory;

    }

    HistoryRecord& operator=(const HistoryRecord& other) {
        if (this == &other)
            return *this;

        HistoryRecord temp(other);
        swap(temp);
        return *this;
    }

    void swap(HistoryRecord& other) {
        using std::swap;
        swap(operationType, other.operationType);
        swap(parameter, other.parameter);
        swap(secondaryParameter, other.secondaryParameter);
        swap(width, other.width);
        swap(height, other.height);
        swap(oldWidth, other.oldWidth);
        swap(oldHeight, other.oldHeight);
        swap(x, other.x);
        swap(y, other.y);
        swap(sequenceNumber, other.sequenceNumber);
        swap(estimatedMemory, other.estimatedMemory);
        swap(oldPixels, other.oldPixels);
        swap(pixelIndices, other.pixelIndices);
        swap(pixelCount, other.pixelCount);
        swap(indexCount, other.indexCount);
        swap(brightnessAdjustments, other.brightnessAdjustments);
        swap(adjustmentCount, other.adjustmentCount);
        swap(brightnessPixelOffsets, other.brightnessPixelOffsets);
        swap(brightnessOldPixels, other.brightnessOldPixels);
        swap(brightnessPixelIndices, other.brightnessPixelIndices);
        swap(brightnessPixelCount, other.brightnessPixelCount);
    }

    void setOldPixels(const Pixel pixels[], int count) {
        delete[] oldPixels;
        oldPixels = nullptr;
        pixelCount = 0;

        if (count > 0 && pixels != nullptr) {
            oldPixels = new Pixel[count];
            for (int i = 0; i < count; i++)
                oldPixels[i] = pixels[i];
            pixelCount = count;
        }
        updateMemoryEstimate();
    }

    void setPixelIndices(const int indices[], int count) {
        delete[] pixelIndices;
        pixelIndices = nullptr;
        indexCount = 0;

        if (count > 0 && indices != nullptr) {
            pixelIndices = new int[count];
            for (int i = 0; i < count; i++)
                pixelIndices[i] = indices[i];
            indexCount = count;
        }
        updateMemoryEstimate();
    }

    void addBrightnessStep(
        int adjustment, const Pixel pixels[],
        const int indices[], int count) {

        int oldSteps = adjustmentCount;
        int oldPixelsCount = brightnessPixelCount;

        int* newAdjustments = new int[oldSteps + 1];
        int* newOffsets = new int[oldSteps + 2];
        Pixel* newPixels = new Pixel[oldPixelsCount + count];
        int* newIndices = new int[oldPixelsCount + count];

        for (int i = 0; i < oldSteps; i++)
            newAdjustments[i] = brightnessAdjustments[i];

        if (oldSteps == 0) {
            newOffsets[0] = 0;
        }
        else {
            for (int i = 0; i <= oldSteps; i++)
                newOffsets[i] = brightnessPixelOffsets[i];
        }

        for (int i = 0; i < oldPixelsCount; i++) {
            newPixels[i] = brightnessOldPixels[i];
            newIndices[i] = brightnessPixelIndices[i];
        }

        for (int i = 0; i < count; i++) {
            newPixels[oldPixelsCount + i] = pixels[i];
            newIndices[oldPixelsCount + i] = indices[i];
        }

        newAdjustments[oldSteps] = adjustment;
        newOffsets[oldSteps + 1] = oldPixelsCount + count;

        delete[] brightnessAdjustments;
        delete[] brightnessPixelOffsets;
        delete[] brightnessOldPixels;
        delete[] brightnessPixelIndices;

        brightnessAdjustments = newAdjustments;
        brightnessPixelOffsets = newOffsets;
        brightnessOldPixels = newPixels;
        brightnessPixelIndices = newIndices;
        adjustmentCount++;
        brightnessPixelCount += count;
        parameter += adjustment;

        updateMemoryEstimate();
    }

    void addBrightnessAdjustment(int adjustment) {
        Pixel unusedPixel = {};
        int unusedIndex = 0;
        addBrightnessStep(adjustment, &unusedPixel, &unusedIndex, 0);
    }

    void mergeBrightnessRecord(const HistoryRecord& other) {
        if (operationType != BRIGHTNESS ||
            other.operationType != BRIGHTNESS)
            return;

        if (other.adjustmentCount == 0) {
            addBrightnessStep(other.parameter, nullptr, nullptr, 0);
            return;
        }

        for (int i = 0; i < other.adjustmentCount; i++) {
            int start = other.brightnessPixelOffsets[i];
            int end = other.brightnessPixelOffsets[i + 1];
            addBrightnessStep(
                other.brightnessAdjustments[i],
                other.brightnessOldPixels + start,
                other.brightnessPixelIndices + start,
                end - start);
        }
    }

    void updateMemoryEstimate() {
        estimatedMemory = sizeof(HistoryRecord);
        estimatedMemory += static_cast<long long>(pixelCount) * sizeof(Pixel);
        estimatedMemory += static_cast<long long>(indexCount) * sizeof(int);
        estimatedMemory += static_cast<long long>(adjustmentCount) * sizeof(int);
        estimatedMemory += static_cast<long long>(adjustmentCount + 1) * sizeof(int);
        estimatedMemory += static_cast<long long>(brightnessPixelCount) *
            (sizeof(Pixel) + sizeof(int));
    }
};
