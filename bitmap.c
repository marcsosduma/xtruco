/*
bitmap.c
Useful routines for manipulating X11 bitmaps
*/

#include "xbook.h"

Pixmap LoadBitmap( Display *display, Window window, char filename[], unsigned int *width, unsigned int * height )
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
