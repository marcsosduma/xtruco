/* cursor.c
C routines to enable an X11 cursor for a window.
*/

#include "xbook.h"

Cursor MakeCursor( display, window, which_cursor )
Display		*display;
Window		window;
unsigned int	which_cursor;
{
	Cursor cursor;

	cursor = XCreateFontCursor( display, (unsigned) which_cursor );
	if( cursor != (Cursor) None )
	{
		XDefineCursor( display, window, cursor );
	}
	return ( cursor );
}



