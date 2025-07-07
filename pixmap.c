/*
pixmap.c
routines for creating and deleting X11 pixmaps.
*/

#include "xbook.h"

int ClearPixmap( Display * display, Pixmap pixmap, GC gc, unsigned long fore,
	unsigned long back, int width, int height )
{
	XSetForeground( display, gc, back );
	XFillRectangle( display, pixmap, gc, 
		0, 0, width, height );
	XSetForeground( display, gc, fore );
}


Pixmap CreatePixmap( Display *display, Window window, int width, int height, 
	int depth, unsigned long fore, unsigned long back, GC * gc)
{
	Pixmap pixmap;

	pixmap = XCreatePixmap( display, window,
			width, height, depth );
	if( pixmap == (Pixmap) None )
	{
		QuitX( display,
			"ERROR: Could not create pixmap",
			 " " );
	}
	*gc = MakeGC( display, pixmap, fore, back );
	ClearPixmap( display, pixmap, *gc, fore, back, width, height );
	return( pixmap );
}

