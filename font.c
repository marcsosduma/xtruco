/*
font.c
X11 routines for loading a font
*/

#include "xbook.h"

#define BUFSIZE 256

int CheckFontName(int argc, char *argv[], char fontname[]) {
    for (int i = 1; i < argc; i++) {
        // Verifica -font (5 chars) ou -fn (3 chars)
        if ((strncmp(argv[i], "-font", 5) == 0) || (strncmp(argv[i], "-fn", 3) == 0)) {
            if ((i + 1) < argc) {
                strncpy(fontname, argv[i + 1], BUFSIZE);
                fontname[BUFSIZE] = '\0'; // segurança para evitar overflow
                return 1;
            } else {
                fprintf(stderr, "Error: usage is -font FontName\n");
                return 0;
            }
        }
    }
    return 0;
}

XFontStruct *LoadFont(Display *display, GC gc, int argc, char *argv[], const char default_name[]) {
    XFontStruct *font;
    char name[BUFSIZE + 1];

    strncpy(name, default_name, BUFSIZE);
    name[BUFSIZE] = '\0';

    CheckFontName(argc, argv, name);

    font = XLoadQueryFont(display, name);
    if (!font) {
        font = XLoadQueryFont(display, default_name);
        if (!font) {
            font = XLoadQueryFont(display, "fixed");
        }
    }

    if (font) {
        XSetFont(display, gc, font->fid);
    } else {
        fprintf(stderr, "Error: could not load any font (tried \"%s\", \"%s\", \"fixed\")\n", name, default_name);
        return NULL;
    }

    return font;
}
