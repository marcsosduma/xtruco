#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Display *display;
Window window;
GC gc;

// Função para carregar uma imagem BMP
Pixmap LoadBMP(Display *display, Window window, const char *filename, int *width, int *height);

int main() {
    int screenWidth, screenHeight;
    int imageWidth, imageHeight;
    Pixmap imagePixmap;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Erro ao abrir a exibição\n");
        exit(1);
    }

    int screen = DefaultScreen(display);
    screenWidth = DisplayWidth(display, screen);
    screenHeight = DisplayHeight(display, screen);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, screenWidth, screenHeight, 0,
                                  BlackPixel(display, screen), WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, 0);

    // Carregar a imagem BMP
    imagePixmap = LoadBMP(display, window, "example.bmp", &imageWidth, &imageHeight);

    // Desenhar a imagem no centro da tela
    int x = (screenWidth - imageWidth) / 2;
    int y = (screenHeight - imageHeight) / 2;
    XCopyArea(display, imagePixmap, window, gc, 0, 0, imageWidth, imageHeight, x, y);

    XFlush(display);

    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress)
            break;
    }

    XFreePixmap(display, imagePixmap);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
