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

int SetNormalHints( Display *display, Window window, int x, int y, int width, int height);

int CreateButton( Display *display, Window window, int x, int y, unsigned long fore, unsigned long back, 
			  Font font_id, char text[], int (*function) ());

Bool InvButton(Display *display, Window window, int x, int y, int c, int a,
               int (*function)(Display *, Window, int));

int InvEvent( Display *display,  XEvent	*event);

int ButtonEvent( Display *display, XEvent *event);

int CheckEvent( Display *display, int want_exposes, int width, int height, XEvent *event, KeySym *keysym);

int CloseDisplay( Display *display, Window window, GC gc);

int MapWindow( Display *display, Window window);

int NameWindow( Display *display, Window window, char *name, char *class_name, char *class_type);

int SetWMHints( Display *display, Window window, Pixmap icon);