#include <stdlib.h>
#include "xbook.h"

#define MAX_BUTTONS 20
#define TEXT_MARGIN 4

typedef struct {
    Display *display;
    Window w;
    Window parent;
    GC gc;
    unsigned long fore, back;
    char text[BUFSIZE + 1];
    int length;
    int (*callback)(Display *, Window);
	int hovered; 
} Button;

static Button buttons[MAX_BUTTONS];
static int button_count = 0;

int CreateButton(Display *display, Window parent, int x, int y,
                 unsigned long fore, unsigned long back, Font font,
                 const char *text, int (*callback)(Display *, Window)) {
    
    if (button_count >= MAX_BUTTONS) return False;

    Window w = CreateWindow(display, parent, x, y, BUTTON_WIDTH, BUTTON_HEIGHT, 0, fore, back,
                        ExposureMask | ButtonPressMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask);
    GC gc = MakeGC(display, w, fore, back);
    XSetFont(display, gc, font);

    Button *btn = &buttons[button_count];
    btn->display = display;
    btn->w = w;
    btn->parent = parent;
    btn->gc = gc;
    btn->fore = fore;
    btn->back = back;
    btn->callback = callback;

    strncpy(btn->text, text, BUFSIZE);
    btn->text[BUFSIZE] = '\0';
    btn->length = strlen(btn->text);

	XMapRaised(display, w);
    XFlush(display);
    button_count++;
    return True;
}

void Draw3DEffect(Display *display, Window win, GC gc,
                  unsigned long light, unsigned long shadow, int pressed) {
    XSetForeground(display, gc, pressed ? shadow : light);
    XDrawLine(display, win, gc, 0, 0, BUTTON_WIDTH - 1, 0);          // top
    XDrawLine(display, win, gc, 0, 0, 0, BUTTON_HEIGHT - 1);         // left

    XSetForeground(display, gc, pressed ? light : shadow);
    XDrawLine(display, win, gc, 0, BUTTON_HEIGHT - 1, BUTTON_WIDTH - 1, BUTTON_HEIGHT - 1); // bottom
    XDrawLine(display, win, gc, BUTTON_WIDTH - 1, 0, BUTTON_WIDTH - 1, BUTTON_HEIGHT - 1);  // right
}

void DrawButtonText(Display *display, Window win, GC gc, const char *text, int length) {
    XFontStruct *font = XQueryFont(display, XGContextFromGC(gc));
    int text_width = XTextWidth(font, text, length);
    int x = (BUTTON_WIDTH - text_width) / 2;
    int y = (BUTTON_HEIGHT + font->ascent - font->descent) / 2;
    XDrawString(display, win, gc, x, y, text, length);
}

void RedrawButton(Display *display, int index, int pressed) {
    Button *btn = &buttons[index];

    unsigned long bg_color = btn->back;
    if (btn->hovered && !pressed) {
        bg_color = btn->back ^ 0x303030; // cria uma cor mais clara (efeito hover)
    }

    XSetForeground(display, btn->gc, bg_color);
    XFillRectangle(display, btn->w, btn->gc, 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);

    Draw3DEffect(display, btn->w, btn->gc, WhitePixel(display, 0), BlackPixel(display, 0), pressed);
    XSetForeground(display, btn->gc, btn->fore);
    DrawButtonText(display, btn->w, btn->gc, btn->text, btn->length);
    XFlush(display);
}

int FindButton(Display *display, Window win) {
    for (int i = 0; i < button_count; i++) {
        if (buttons[i].display == display && buttons[i].w == win) {
            return i;
        }
    }
    return -1;
}

void play_button_sound() {
    const char *sound = "/usr/share/sounds/freedesktop/stereo/message.oga";
    if (access(sound, F_OK) == 0) {
        int rc = system("paplay /usr/share/sounds/freedesktop/stereo/message.oga 2>/dev/null &");
        (void)rc;
    } else {
        fprintf(stderr, "Som padrão não encontrado: %s\n", sound);
    }
}

void ExecuteButton(Display *display, int index) {
    RedrawButton(display, index, True);
    XFlush(display);
    usleep(100000);

    play_button_sound();
    if (buttons[index].callback) {
        buttons[index].callback(display, buttons[index].parent);
    }

    RedrawButton(display, index, False);
}

int ButtonEvent(Display *display, XEvent *event) {
    int index = -1;

    switch (event->type) {
        case Expose:
            index = FindButton(display, event->xexpose.window);
            if (index >= 0) {
                RedrawButton(display, index, False);
                return True;
            }
            break;
        case ButtonPress:
            index = FindButton(display, event->xbutton.window);
            if (index >= 0) {
                ExecuteButton(display, index);
                return True;
            }
            break;
        case EnterNotify:
            index = FindButton(display, event->xcrossing.window);
            if (index >= 0) {
                buttons[index].hovered = 1;
                RedrawButton(display, index, False);
                return True;
            }
            break;
        case LeaveNotify:
            index = FindButton(display, event->xcrossing.window);
            if (index >= 0) {
                buttons[index].hovered = 0;
                RedrawButton(display, index, False);
                return True;
            }
            break;
    }

    return False;
}
