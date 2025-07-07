/*
Includes the most common X11
header files.
*/

#define i386 1
#define X11R4 1

#include <stdio.h>
#include <memory.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>

#ifndef BUFSIZE
#define BUFSIZE 256
#endif

#define FULL_LENGTH	8192L

#define PROP_NAME "__XBOOK"

#define EVENT_MASK	(long)( ButtonPressMask	|\
				KeyPressMask	|\
				ExposureMask	|\
				StructureNotifyMask )

#define DEFAULT_CURSOR	XC_left_ptr

#define BUTTON_WIDTH	70
#define BUTTON_HEIGHT	24

Atom ConvertTarget();

Cursor MakeCursor();

Display *SetUpDisplay();
Display *OpenDisplay();

GC MakeGC();

KeySym Key2Keysym();

Pixmap CreatePixmap();
Pixmap LoadBitmap();

Time LastTimeStamp();
Time PrimaryTimeStamp();

unsigned long Bytes2Long();
unsigned long GetColor();

Window BuildWList();
Window CheckWindow();
Window CreateWindow();

Window FindWindow();
Window FindSendWindow();
Window FindSubwindow();

Window GetParent();

Window PickWindow();
Window SearchWindowTree();
Window TopWindow();

XFontStruct *LoadFont();

XImage* loadBitmapFromMemory(Display* display, Visual* visual, int screen,
                             const unsigned char* bmpData, unsigned int bmpSize);

int CheckGeometry(  int argc, char *argv[], int screen_width, int screen_height, 
					int *x, int *y, int *width, int *height);

int SetGC( Display *display, GC	gc, unsigned long fore, unsigned long back);

int QuitX( Display *display, char error_message[], char error_file[]);