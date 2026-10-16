
#pragma once

struct ProcessingJob {
    int type;
    int width;
    int height;
    int x;
    int y;
    int radius;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    char filename[256];
    bool cancelled;

    ProcessingJob() {
        type = 1;
        width = 0;
        height = 0;
        x = 0;
        y = 0;
        radius = 0;
        red = 0;
        green = 0;
        blue = 0;
        filename[0] = '\0';
        cancelled = false;
    }
};
