/*
gc.c
X11 C routines to create a graphics context
and fill the screen with colored fields
*/

#include "xbook.h"

GC MakeGC( display, drawable, fore, back )
Display		*display;
Drawable	drawable;
unsigned long	fore, back;
{
	GC	gc;
	XGCValues	gcvalues;

	gcvalues.foreground = fore;
	gcvalues.background = back;
	gc = XCreateGC( display, drawable,
			( GCForeground | GCBackground ),
			&gcvalues );
	if( gc==0 )
	{
		QuitX( display,
			"Error in creating a Graphics context", "" );
	}
	return (gc);
}

SetGC( display, gc, fore, back )
Display *display;
GC	gc;
unsigned long	fore, back;
{
	XSetForeground( display, gc, fore );
	XSetBackground( display, gc, back );
}

