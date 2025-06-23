/*
button.c
X11 routines for buttons to handle simple user inputs
through push-button.
*/

#include "xbook.h"

typedef struct
	{
	Display		*display;
	Window		w;
	Window		parent;
	GC		gc;
	unsigned long	fore, back;
	char		text[ BUFSIZE + 1 ];
	int		length;
	int		(*function)();
	}	ButtonStruct;

#define MAX_BUTTONS 20

ButtonStruct	AppButtons[ MAX_BUTTONS + 1 ];
int		buttons_used = 0;

CreateButton( display, window, x, y, fore, back, font_id, text, function )
Display		*display;
Window		window;
int		x, y;
unsigned long	fore, back;
Font		font_id;
char		text[];
int		(*function) ();
{
	Window	w;
	GC	gc;

	if( buttons_used < ( MAX_BUTTONS - 1) )
	{
		w = CreateWindow( display, window, x, y,
			BUTTON_WIDTH, BUTTON_HEIGHT, 0,
			fore, back,
			ExposureMask | ButtonPressMask );
		gc = MakeGC( display, w, fore, back );
		XSetFont( display, gc, font_id );
		AppButtons[ buttons_used ].display	= display;
		AppButtons[ buttons_used ].w		= w;
		AppButtons[ buttons_used ].parent	= window;
		AppButtons[ buttons_used ].gc		= gc;
		AppButtons[ buttons_used ].fore		= fore;
		AppButtons[ buttons_used ].back		= back;
		AppButtons[ buttons_used ].function	= function;
		if( strlen( text ) < BUFSIZE )
		{
			(void) strcpy( AppButtons[ buttons_used ]. text, text );
		}
		else
		{
			(void) strncpy( AppButtons[ buttons_used ].text, text,
				BUFSIZE );
			AppButtons[ buttons_used ].text[ BUFSIZE ] = '\0';
		}
		AppButtons[ buttons_used ].length =
			strlen( AppButtons[ buttons_used ].text );
		XFlush( display );
		buttons_used++;
		return( True );
	}
	return( False);
}

ButtonEvent( display, event )
Display	*display;
XEvent	*event;
{
	int	which_button;
	
	switch( event->type )
	{
		case Expose:
			which_button = ButtonFind( display,
					event->xexpose.window );
			if(which_button >= 0 )
			{
				ButtonRedraw( display, which_button );
				return ( True );
			}
			break;
		case ButtonPress:
			which_button = ButtonFind( display,
					event->xbutton.window );
			if( which_button >= 0 )
			{
				ButtonExec( display, which_button );
				return( True );
			}
			break;
	}
	XFlush( display );
	return( False );
}

ButtonExec( display, which_button )
Display	*display;
int	which_button;
{
	ButtonHighlight( display, which_button );
	(AppButtons[ which_button ].function)(display,
		AppButtons[ which_button ].parent );
	XClearWindow( display,
		AppButtons[ which_button ].w );
	ButtonRedraw( display, which_button );
}

ButtonHighlight( display, which_button )
Display	*display;
int	which_button;
{
	XFillRectangle( display,
		AppButtons[ which_button ].w,
		AppButtons[ which_button ].gc,
		0, 0,
		BUTTON_WIDTH - 5, BUTTON_HEIGHT - 5 );
	SetGC( display,
		AppButtons[ which_button ].gc,
		AppButtons[ which_button ].back,
		AppButtons[ which_button ].fore );
	ButtonText( display,
		AppButtons[ which_button ].w,
		AppButtons[ which_button ].gc,
		AppButtons[ which_button ].text,
		AppButtons[ which_button ].length );
	SetGC( display,
		AppButtons[ which_button ].gc,
		AppButtons[ which_button ].fore,
		AppButtons[ which_button ].back );
	XFlush( display );
}

ButtonRedraw( display, which_button )
Display	*display;
int	which_button;
{
	XDrawRectangle( display,
		AppButtons[ which_button ].w,
		AppButtons[ which_button ].gc,
		0, 0,
		BUTTON_WIDTH - 6, BUTTON_HEIGHT - 6 );
	ButtonText( display,
		AppButtons[ which_button ].w,
		AppButtons[ which_button ].gc,
		AppButtons[ which_button ].text,
		AppButtons[ which_button ].length );
}

ButtonText( display, window, gc, text, length )
Display	*display;
Window	window;
GC	gc;
char	text[];
int	length;
{
	XDrawImageString( display, window, gc,
		5, 15,
		text, length );
}

ButtonFind( display, window )
Display *display;
Window	window;
{
	int which_button;

	for( which_button = 0; which_button < buttons_used; which_button++ )
	{
		if( ( window == AppButtons[ which_button ].w ) &&
		  ( display == AppButtons[ which_button ].display ) )
		{
			return( which_button );
		}
	}
	return ( -1 );
}
