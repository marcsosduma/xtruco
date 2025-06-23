/*
bitmap.c
Useful routines for manipulating X11 bitmaps
*/

#include "xbook.h"

Pixmap LoadBitmap( display, window, filename, width, height )
Display    *display;
Window     window;
char       filename[];
int        *width, *height;
{
    int status;
    int xHotSpot, yHotSpot;
    Pixmap pixmap;

    status = XReadBitmapFile( display, window,
				       filename,
				       width, height,
				       &pixmap,
				       &xHotSpot, &yHotSpot );
    if( status != BitmapSuccess )
    {
        QuitX( display, "Nao pode ler bitmap ", filename );
    }
    return( pixmap );
}
