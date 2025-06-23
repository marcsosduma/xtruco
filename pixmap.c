/*
pixmap.c
routines for creating and deleting X11 pixmaps.
*/

#include "xbook.h"

Pixmap CreatePixmap( display, window, width, height, depth, fore, back, gc)
Display		*display;
Window		window;
int		width, height, depth;
unsigned long	fore, back;
GC		*gc;
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

ClearPixmap( display, pixmap, gc, fore, back, width, height )
Display		*display;
Pixmap		pixmap;
GC		gc;
unsigned long	fore, back;
int		width, height;
{
	XSetForeground( display, gc, back );
	XFillRectangle( display, pixmap, gc, 
		0, 0, width, height );
	XSetForeground( display, gc, fore );
}

