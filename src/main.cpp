#include <iostream>
#include "Image.h"
#include "HistoryManager.h"
#include "stackType.h"
#include "queueType.h"
#include "ProcessingJob.h"
#include "HistoryController.h"
using namespace std;

int main() {
    Image img;
    stackType<Image> undoStack;
    stackType<Image> redoStack;
    queueType<ProcessingJob> processingQueue(20);
    HistoryController history(50LL * 1024 * 1024);
    int choice;
    bool loaded = false;

    do {
        cout << "\n========== PIXELFORGE ==========\n";
        cout << "1. Load Image\n";
        cout << "2. Show Image Information\n";
        cout << "3. Adjust Brightness\n";
        cout << "4. Convert to Grayscale\n";
        cout << "5. Flip Horizontally\n";
        cout << "6. Flip Vertically\n";
        cout << "7. Rotate 90 Degrees\n";
        cout << "8. Rotate 180 Degrees\n";
        cout << "9. Rotate 270 Degrees\n";
        cout << "10. Crop Image\n";
        cout << "11. Save Image\n";
        cout << "12. Undo\n";
        cout << "13. Redo\n";
        cout << "14. Add Test Job\n";
        cout << "15. List Pending Jobs\n";
        cout << "16. Process Next Job\n";
        cout << "17. Cancel a Pending Job\n";
        cout << "18. Clear Pending Jobs\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Exiting.\n";
            break;
        }

        if (choice == 0) {
            cout << "Exiting PixelForge.\n";
            break;
        }

        if (choice == 14) {
            if (processingQueue.isFullQueue()) {
                cout << "Processing queue is full. Process pending jobs first.\n";
                continue;
            }

            ProcessingJob job;
            cout << "Select job type:\n";
            cout << "1. Resize\n";
            cout << "2. Blur\n";
            cout << "3. Stack-based Bucket Fill\n";
            cout << "4. Queue-based Bucket Fill\n";
            cout << "5. Export\n";
            cout << "Enter job type: ";
            cin >> job.type;

            if (!cin) {
                cout << "Invalid input. Exiting.\n";
                break;
            }

            if (job.type < 1 || job.type > 5) {
                cout << "Invalid job type.\n";
                continue;
            }

            if (job.type == 1) {
                cout << "Enter new width and height: ";
                cin >> job.width >> job.height;
            }
            else if (job.type == 2) {
                cout << "Enter blur radius: ";
                cin >> job.radius;
            }
            else if (job.type == 3 || job.type == 4) {
                cout << "Enter x, y, red, green, blue: ";
                int r, g, b;
                cin >> job.x >> job.y >> r >> g >> b;

                if (cin && r >= 0 && r <= 255 &&
                    g >= 0 && g <= 255 &&
                    b >= 0 && b <= 255) {
                    job.red = (unsigned char)r;
                    job.green = (unsigned char)g;
                    job.blue = (unsigned char)b;
                }
                else {
                    cout << "Invalid coordinates or colour values.\n";
                    continue;
                }
            }
            else if (job.type == 5) {
                cout << "Enter output image path: ";
                cin >> job.filename;
            }
            if (!cin) {
                cout << "Invalid input. Exiting.\n";
                break;
            }

            processingQueue.addQueue(job);
            cout << "Job added to processing queue.\n";
            cout << "Pending jobs: " << processingQueue.size() << endl;
            continue;
        }

        if (choice == 15) {
            if (processingQueue.isEmptyQueue()) {
                cout << "No pending jobs.\n";
                continue;
            }

            cout << "\nPending jobs (front to rear):\n";

            for (int i = 0; i < processingQueue.size(); i++) {
                ProcessingJob job = processingQueue.getAt(i);

                cout << i + 1 << ". ";

                if (job.cancelled) {
                    cout << "[CANCELLED] ";
                }

                switch (job.type) {
                    case 1: cout << "Resize"; break;
                    case 2: cout << "Blur"; break;
                    case 3: cout << "Stack-based Bucket Fill"; break;
                    case 4: cout << "Queue-based Bucket Fill"; break;
                    case 5: cout << "Export"; break;
                    default: cout << "Unknown job"; break;
                }

                cout << endl;
            }

            continue;
        }

        if (choice == 16) {
            if (processingQueue.isEmptyQueue()) {
                cout << "No pending jobs to process.\n";
                continue;
            }

            ProcessingJob job = processingQueue.front();
            processingQueue.deleteQueue();

            if (job.cancelled) {
                cout << "Job was cancelled. Skipping.\n";
                continue;
            }

            if (job.type == 1) {
                if (!loaded) {
                    cout << "Load an image before processing resize.\n";
                    continue;
                }

                try {
                    undoStack.push(img);
                    redoStack.initializeStack();
                    img.resize(job.width, job.height);
                    cout << "Resize job completed.\n";
                }
                catch (const exception& e) {
                    cout << "Resize job failed: " << e.what() << endl;
                }
            }
            else if (job.type == 2) {
                if (!loaded) {
                    cout << "Load an image before processing blur.\n";
                    continue;
                }

                try {
                    undoStack.push(img);
                    redoStack.initializeStack();
                    img.blur(job.radius);
                    cout << "Blur job completed.\n";
                }
                catch (const exception& e) {
                    cout << "Blur job failed: " << e.what() << endl;
                }
            }
            else if (job.type == 5) {
                if (!loaded) {
                    cout << "Load an image before exporting.\n";
                    continue;
                }

                if (img.saveImage(job.filename))
                    cout << "Export job completed.\n";
                else
                    cout << "Export job failed.\n";
            }
            else if (job.type == 3 || job.type == 4) {
                if (!loaded) {
                    cout << "Load an image before processing bucket fill.\n";
                    continue;
                }

                try {
                    if (job.x < 0 || job.x >= img.getWidth() ||
                        job.y < 0 || job.y >= img.getHeight()) {
                        cout << "Bucket-fill coordinates are out of range.\n";
                        continue;
                    }

                    Pixel newColor;
                    newColor.red = job.red;
                    newColor.green = job.green;
                    newColor.blue = job.blue;

                    undoStack.push(img);
                    redoStack.initializeStack();

                    if (job.type == 3) {
                        stackType<int> pixelStack;
                        img.bucketFillWithStack(
                            job.x, job.y, newColor, pixelStack);

                        cout << "Stack-based bucket fill completed.\n";
                    }
                    else {
                        queueType<int> pixelQueue(
                            img.getWidth() * img.getHeight());

                        img.bucketFillWithQueue(
                            job.x, job.y, newColor, pixelQueue);

                        cout << "Queue-based bucket fill completed.\n";
                    }
                }
                catch (const exception& e) {
                    cout << "Bucket fill failed: " << e.what() << endl;
                }
            }

            else {
                cout << "Unknown job type.\n";
            }

            cout << "Remaining jobs: "
                 << processingQueue.size() << endl;
            continue;
        }
        if (choice == 17) {
            if (processingQueue.isEmptyQueue()) {
                cout << "No pending jobs to cancel.\n";
                continue;
            }

            int totalJobs = processingQueue.size();

            cout << "Pending jobs: " << totalJobs << endl;
            cout << "Enter job position to cancel: ";

            int position;
            cin >> position;

            if (!cin) {
                cout << "Invalid input. Exiting.\n";
                break;
            }

            if (position < 1 || position > totalJobs) {
                cout << "Invalid job position.\n";
                continue;
            }

            queueType<ProcessingJob> tempQueue(
                processingQueue.capacity());

            for (int i = 1; i <= totalJobs; i++) {
                ProcessingJob job = processingQueue.front();
                processingQueue.deleteQueue();

                if (i != position) {
                    tempQueue.addQueue(job);
                }
                else {
                    cout << "Cancelled job at position "
                         << position << ".\n";
                }
            }

            while (!tempQueue.isEmptyQueue()) {
                ProcessingJob job = tempQueue.front();
                tempQueue.deleteQueue();
                processingQueue.addQueue(job);
            }

            cout << "Remaining jobs: "
                 << processingQueue.size() << endl;
            continue;
        }

        if (choice == 18) {
            int removedJobs = processingQueue.size();
            processingQueue.initializeQueue();

            cout << "Cleared " << removedJobs
                 << " pending job(s).\n";
            continue;
        }

        if (choice == 1) {
            char filename[256];
            cout << "Enter image path: ";
            cin >> filename;

            if (img.loadImage(filename)) {
                loaded = true;
                undoStack.initializeStack();
                redoStack.initializeStack();

                cout << "Image loaded successfully!\n";
                cout << "Dimensions: " << img.getWidth()
                     << " x " << img.getHeight() << endl;
            }
            else {
                cout << "Failed to load image.\n";
            }

            continue;
        }

        if (!loaded) {
            cout << "Please load an image first.\n";
            continue;
        }

        if (choice == 2) {
            cout << "Width: " << img.getWidth() << endl;
            cout << "Height: " << img.getHeight() << endl;
            cout << "Undo history: " << undoStack.size() << endl;
            cout << "Redo history: " << redoStack.size() << endl;
            continue;
        }

        if (choice == 11) {
            char filename[256];
            cout << "Enter output image path: ";
            cin >> filename;

            if (img.saveImage(filename))
                cout << "Image saved successfully.\n";
            else
                cout << "Failed to save image.\n";

            continue;
        }

        if (choice == 12) {
            if (undoStack.isEmptyStack()) {
                cout << "Nothing to undo.\n";
            }
            else {
                redoStack.push(img);
                img = undoStack.top();
                undoStack.pop();
                cout << "Undo successful.\n";
            }

            continue;
        }

        if (choice == 13) {
            if (redoStack.isEmptyStack()) {
                cout << "Nothing to redo.\n";
            }
            else {
                undoStack.push(img);
                img = redoStack.top();
                redoStack.pop();
                cout << "Redo successful.\n";
            }

            continue;
        }

        if (choice < 3 || choice > 10) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        try {
            if (choice == 3) {
                int amount;
                cout << "Enter brightness adjustment: ";
                cin >> amount;
                if (!cin) {
                    cout << "Invalid input.\n";
                    break;
                }

                undoStack.push(img);
                redoStack.initializeStack();
                img.adjustBrightness(amount);
                cout << "Brightness adjusted.\n";
            }
            else if (choice == 4) {
                undoStack.push(img);
                redoStack.initializeStack();
                img.convertToGrayscale();
                cout << "Image converted to grayscale.\n";
            }
            else if (choice == 5) {
                undoStack.push(img);
                redoStack.initializeStack();
                img.flipHorizontal();
                cout << "Image flipped horizontally.\n";
            }
            else if (choice == 6) {
                undoStack.push(img);
                redoStack.initializeStack();
                img.flipVertical();
                cout << "Image flipped vertically.\n";
            }
            else if (choice == 7) {
                undoStack.push(img);
                redoStack.initializeStack();
                img.rotate90();
                cout << "Image rotated 90 degrees.\n";
            }
            else if (choice == 8) {
                undoStack.push(img);
                redoStack.initializeStack();
                img.rotate180();
                cout << "Image rotated 180 degrees.\n";
            }
            else if (choice == 9) {
                undoStack.push(img);
                redoStack.initializeStack();
                img.rotate270();
                cout << "Image rotated 270 degrees.\n";
            }
            else if (choice == 10) {
                int x, y, w, h;
                cout << "Enter x, y, width, height: ";
                cin >> x >> y >> w >> h;

                if (!cin) {
                    cout << "Invalid input.\n";
                    break;
                }

                undoStack.push(img);
                redoStack.initializeStack();
                img.crop(x, y, w, h);
                cout << "Crop operation completed.\n";
            }
        }
        catch (const exception& e) {
            cout << "Operation failed: " << e.what() << endl;
        }

    } while (true);

    return 0;
}
