/*
gc.c
X11 C routines to create a graphics context
and fill the screen with colored fields
*/

#include "xbook.h"

GC MakeGC( Display *display, Drawable drawable, unsigned long fore, unsigned long back)
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

int SetGC( Display *display, GC	gc, unsigned long fore, unsigned long back)
{
	XSetForeground( display, gc, fore );
	XSetBackground( display, gc, back );
}

