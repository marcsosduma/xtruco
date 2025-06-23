/*
// window.c
// X11 routines for high-level windows
// of an application.
*/

#include "xbook.h"

char AppName[ BUFSIZE + 1 ];

Window CreateWindow( display, parent, x, y, width, height, border, fore, back,
events )
Display	*display;
Window	parent;
int	x, y, width, height, border;
unsigned int	fore, back;
long	events;
{
	Window	window;
	XSetWindowAttributes	attributes;
	unsigned long		attribute_mask;
	Visual			*visual = CopyFromParent;

	attributes.background_pixel	= back;
	attributes.border_pixel		= fore;
	attributes.event_mask		= events;
	attribute_mask			= CWBackPixel | CWBorderPixel |
						CWEventMask;

	window = XCreateWindow( display,
				parent,
				x, y, width, height,
				border,
				CopyFromParent,
				InputOutput,
				visual,
				attribute_mask,
				&attributes );

	if( window == (Window) None )
	{
		QuitX( display, "",
			"Error: Could not open window." );
	}
	return( window );
}

CheckGeometry( argc, argv, screen_width, screen_height, x, y, width, height )
int argc;
char *argv[];
int screen_width, screen_height;
int *x, *y, *width, *height;
{
	int status;
	int x1, y1, width1, height1;
	int counter;

	counter = 1;
	while( counter < argc )
	{
		if( strncmp( argv[ counter ], "-geom", 5 ) == 0)
		{
			counter++;
			if( counter < argc )
			{
				status = XParseGeometry( argv[ counter ],
					&x1, &y1, &width1, &height1 );
				if( status & XValue )
					*x = x1;
				if( status & YValue )
					*y = y1;
				if( status & WidthValue )
					*width = width1;
				if( status & HeightValue )
					*height = height1;
				if( status & XNegative )
					*x = screen_width - *width + *x;
				if( status & YNegative )
					*y = screen_height - *height + *y;
			}
		}
		counter++;
	}
}

MapWindow( display, window )
Display *display;
Window window;
{
	XMapRaised( display, window );
	XMapSubwindows( display, window );
	XFlush( display );
}

NameWindow( display, window, name, class_name, class_type )
Display *display;
Window window;
char *name;
char *class_name;
char *class_type;
{
	XClassHint	*classhints;
#ifdef X11R4
	classhints = XAllocClassHint();
#else
	classhints = (XClassHint *) malloc( sizeof( XClassHint ) );
#endif
	if( classhints != (XClassHint *) NULL )
	{
		classhints->res_name = class_name;
		classhints->res_class = class_type;
		XSetClassHint( display, window, classhints );
#ifdef X11R4
	XFree( classhints );
#else
	free( classhints );
#endif
	}
	XStoreName( display, window, name );
	XSetIconName( display, window, name );
	(void) strcpy( AppName, name );
}

SetWMHints( display, window, icon )
Display *display;
Window window;
Pixmap icon;
{
	XWMHints	*wmhints;
#ifdef X11R4
	wmhints	=	XAllocWMHints();
#else
	wmhints =	(XWMHints *) malloc( sizeof( XWMHints ) );
#endif
	if( wmhints != (XWMHints *) NULL )
	{
		wmhints->initial_state 	= NormalState;
		wmhints->input		= True;
		if( icon != (Pixmap) None )
		{
			wmhints->icon_pixmap	= icon;
			wmhints->icon_mask	= icon;
			wmhints->flags		= StateHint	|
						  InputHint	|
						  IconPixmapHint |
						  IconMaskHint;
		}
		else
		{
			wmhints->flags	= StateHint | InputHint;
		}
		XSetWMHints( display, window, wmhints );
#ifdef X11R6
		XFree( wmhints );
#else
		free( wmhints );
#endif
	}
}

SetNormalHints( display, window, x, y, width, height )
Display		*display;
Window		window;
int		x, y, width, height;
{
	XSizeHints	*sizehints;
#ifdef X11R4
	sizehints = XAllocSizeHints();
#else
	sizehints = (XSizeHints *) malloc( sizeof( XSizeHints ) );
#endif
	if( sizehints != (XSizeHints *) NULL )
	{
		sizehints->x		=x;
		sizehints->y		=y;
		sizehints->width	=width;
		sizehints->height	=height;
		sizehints->min_width	=100;
		sizehints->min_height	=50;
		sizehints->flags	=USPosition | USSize | PMinSize;
#ifdef X11R4
		sizehints->base_width	=width;
		sizehints->base_height	=height;
		sizehints->flags	|= PBaseSize;
		XSetWMNormalHints( display, window, sizehints );
		XFree( sizehints );
#else
		XSetNormalHints( display, window, sizehints );
		free( sizehints );
#endif
	}
}

GetAppName( name )
char *name;
{
	(void) strcpy(name , AppName );
}
