#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// xxd -i cards.bmp > cards.h  ---gerar o .h

typedef unsigned char byte;

unsigned long rgbToPixel(XImage* image, byte r, byte g, byte b) {
    return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}

XImage* loadBitmapFromMemory(Display* display, Visual* visual, int screen,
                             const unsigned char* bmpData, unsigned int bmpSize) {
    if (!bmpData || bmpSize < 54) {
        fprintf(stderr, "Dados BMP inválidos ou muito pequenos\n");
        return NULL;
    }

    if (bmpData[0] != 'B' || bmpData[1] != 'M') {
        fprintf(stderr, "Assinatura BMP inválida\n");
        return NULL;
    }

    int width = *(int*)(bmpData + 18);
    int height = *(int*)(bmpData + 22);
    short bitCount = *(short*)(bmpData + 28);
    int dataOffset = *(int*)(bmpData + 10);

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Dimensões inválidas\n");
        return NULL;
    }

    int row_padded;
    const unsigned char* pixelData;
    const unsigned char* palette = NULL;

    if (bitCount == 24) {
        row_padded = (width * 3 + 3) & ~3;
        pixelData = bmpData + dataOffset;
    } else if (bitCount == 8) {
        palette = bmpData + 54;
        row_padded = (width + 3) & ~3;
        pixelData = bmpData + 54 + 256 * 4;
    } else {
        fprintf(stderr, "BMP bitCount %d não suportado\n", bitCount);
        return NULL;
    }

    XImage* image = XCreateImage(display, visual,
                                 DefaultDepth(display, screen), ZPixmap, 0,
                                 malloc(width * height * 4), width, height,
                                 32, 0);
    if (!image || !image->data) {
        fprintf(stderr, "Erro ao criar XImage\n");
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        const unsigned char* row = pixelData + (height - 1 - y) * row_padded;
        for (int x = 0; x < width; x++) {
            byte r, g, b;

            if (bitCount == 24) {
                b = row[x * 3 + 0];
                g = row[x * 3 + 1];
                r = row[x * 3 + 2];
            } else { // 8bpp
                byte index = row[x];
                b = palette[index * 4 + 0];
                g = palette[index * 4 + 1];
                r = palette[index * 4 + 2];
            }

            unsigned long pixel = rgbToPixel(image, r, g, b);
            XPutPixel(image, x, y, pixel);
        }
    }

    return image;
}