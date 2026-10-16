
#pragma once
#include "HistoryRecord.h"
#include <stdexcept>

inline HistoryRecord createHistoryRecord(
    const Image& img, int operationType, int parameter = 0) {

    HistoryRecord record;
    record.operationType = operationType;
    record.parameter = operationType == HistoryRecord::BRIGHTNESS ? 0 : parameter;
    record.oldWidth = img.getWidth();
    record.oldHeight = img.getHeight();

    int totalPixels = img.getWidth() * img.getHeight();

    if (operationType == HistoryRecord::BRIGHTNESS) {
        int clippedCount = 0;

        for (int y = 0; y < img.getHeight(); y++) {
            for (int x = 0; x < img.getWidth(); x++) {
                Pixel p = img.getPixel(x, y);
                if ((int)p.red + parameter < 0 || (int)p.red + parameter > 255 ||
                    (int)p.green + parameter < 0 || (int)p.green + parameter > 255 ||
                    (int)p.blue + parameter < 0 || (int)p.blue + parameter > 255)
                    clippedCount++;
            }
        }

        Pixel* oldData = clippedCount > 0 ? new Pixel[clippedCount] : nullptr;
        int* indices = clippedCount > 0 ? new int[clippedCount] : nullptr;
        int position = 0;

        for (int y = 0; y < img.getHeight(); y++) {
            for (int x = 0; x < img.getWidth(); x++) {
                Pixel p = img.getPixel(x, y);
                if ((int)p.red + parameter < 0 || (int)p.red + parameter > 255 ||
                    (int)p.green + parameter < 0 || (int)p.green + parameter > 255 ||
                    (int)p.blue + parameter < 0 || (int)p.blue + parameter > 255) {
                    oldData[position] = p;
                    indices[position] = y * img.getWidth() + x;
                    position++;
                }
            }
        }

        record.addBrightnessStep(parameter, oldData, indices, clippedCount);
        delete[] oldData;
        delete[] indices;
    }
    else if (operationType == HistoryRecord::GRAYSCALE ||
             operationType == HistoryRecord::BLUR ||
             operationType == HistoryRecord::CROP ||
             operationType == HistoryRecord::RESIZE) {

        Pixel* oldData = new Pixel[totalPixels];

        for (int y = 0; y < img.getHeight(); y++)
            for (int x = 0; x < img.getWidth(); x++)
                oldData[y * img.getWidth() + x] = img.getPixel(x, y);

        record.setOldPixels(oldData, totalPixels);
        delete[] oldData;
    }

    record.updateMemoryEstimate();
    return record;
}

inline void applyHistoryOperation(
    Image& img, const HistoryRecord& record, bool undo) {

    
    if (record.operationType == HistoryRecord::BRIGHTNESS) {
        if (undo) {
            for (int step = record.adjustmentCount - 1; step >= 0; step--) {
                img.adjustBrightness(-record.brightnessAdjustments[step]);

                int start = record.brightnessPixelOffsets[step];
                int end = record.brightnessPixelOffsets[step + 1];

                for (int i = start; i < end; i++) {
                    int index = record.brightnessPixelIndices[i];
                    img.setPixel(
                        index % record.oldWidth,
                        index / record.oldWidth,
                        record.brightnessOldPixels[i]);
                }
            }
        }
        else {
            for (int step = 0; step < record.adjustmentCount; step++) {
                img.adjustBrightness(record.brightnessAdjustments[step]);

                int start = record.brightnessPixelOffsets[step];
                int end = record.brightnessPixelOffsets[step + 1];

                for (int i = start; i < end; i++) {
                    int index = record.brightnessPixelIndices[i];
                    Pixel oldPixel = record.brightnessOldPixels[i];
                    int adjustment = record.brightnessAdjustments[step];

                    Pixel newPixel;
                    int r = (int)oldPixel.red + adjustment;
                    int g = (int)oldPixel.green + adjustment;
                    int b = (int)oldPixel.blue + adjustment;

                    newPixel.red = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
                    newPixel.green = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
                    newPixel.blue = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));

                    img.setPixel(
                        index % record.oldWidth,
                        index / record.oldWidth,
                        newPixel);
                }
            }
        }
        return;
    }


    if (undo && record.oldPixels != nullptr) {
        Image restored(record.oldWidth, record.oldHeight);

        for (int i = 0; i < record.pixelCount; i++) {
            int index = record.pixelIndices != nullptr
                ? record.pixelIndices[i] : i;

            restored.setPixel(
                index % record.oldWidth,
                index / record.oldWidth,
                record.oldPixels[i]);
        }

        img = restored;
        return;
    }

    switch (record.operationType) {
    case HistoryRecord::FLIP_HORIZONTAL:
        img.flipHorizontal();
        break;
    case HistoryRecord::FLIP_VERTICAL:
        img.flipVertical();
        break;
    case HistoryRecord::ROTATE_90:
        if (undo) img.rotate270();
        else img.rotate90();
        break;
    case HistoryRecord::ROTATE_180:
        img.rotate180();
        break;
    case HistoryRecord::ROTATE_270:
        if (undo) img.rotate90();
        else img.rotate270();
        break;
    case HistoryRecord::GRAYSCALE:
        if (!undo) img.convertToGrayscale();
        break;
        case HistoryRecord::BLUR:
        if (!undo)
            img.blur(record.parameter);
        break;

    case HistoryRecord::CROP:
        if (!undo)
            img.crop(record.x, record.y, record.width, record.height);
        break;

    case HistoryRecord::RESIZE:
        if (!undo)
            img.resize(record.width, record.height);
        break;

    default:
        throw std::invalid_argument("History operation is not implemented yet");
    }
}
