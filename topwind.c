/*
topwind.c
*/

#include "xbook.h"

Window TopWindow( display, x, y, width, height,
		bitmap_data, bitmap_width, bitmap_height, icon, gc )
Display		*display;
int			x, y, width, height;
char		*bitmap_data;
int			bitmap_width, bitmap_height;
Pixmap		*icon;
GC			*gc;
{
	int		screen;
	Window		rootwindow, window;
	unsigned long	black, white;
	Cursor		cursor;

	screen = DefaultScreen( display );
	rootwindow = RootWindow( display, screen );
	black = BlackPixel( display, screen );
	white = WhitePixel( display, screen );

#define BORDER_WIDTH	2
	
	window = CreateWindow( display,
			rootwindow,
			x, y, width, height,
			BORDER_WIDTH,
			black, white,
			EVENT_MASK );
	*gc = MakeGC( display, window, black, white );
	*icon = XCreateBitmapFromData( display, window,
			bitmap_data,
			bitmap_width, bitmap_height );
	cursor = MakeCursor( display, window, DEFAULT_CURSOR );
	XFreeCursor( display, cursor );
	XFlush( display );
	return( window );
}

