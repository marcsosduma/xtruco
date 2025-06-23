/*
send.c
X11 program to send an event
to another window.....
*/

#include "xbook.h"

#include "sendev.xb"

#define MESSAGE "XSendEvent() Example..."
#define PRESS_MESSAGE "Press the mouse button on the desired window..."

int 	RefreshFlag 	=	False;
int 	QuitFlag	=	False;
GC	AppGC;
char	SendText[ BUFSIZE + 1 ];

main( argc, argv )
int	argc;
char	*argv[];
{
	Display		*display;
	int		screen;
	Window		window;
	int		x, y, width, height;
	XFontStruct	*font;
	Pixmap		icon;
	int		font_height;
	unsigned long	black, white;

	display	= SetUpDisplay( argc, argv, &screen );
	black	= BlackPixel( display, screen );
	white	= WhitePixel( display, screen );
	x	= 10;
	y	= 10;
	width	= 510;
	height	= 80;
	CheckGeometry( argc, argv,
		DisplayWidth( display, screen ),
		DisplayHeight( display, screen ),
		&x, &y, &width, &height );
	window	= TopWindow( display, x, y, width, height,
			sendev_bits, sendev_width, sendev_height,
			&icon,  &AppGC );
	font 	= LoadFont( display, AppGC, argc, argv, "variable" );
	font_height	= (font->ascent + font->descent);
	SetNormalHints( display, window,
		x, y, width, height );
	SetWMHints( display, window, icon );
	NameWindow( display, window, "send", "send", "Send" );
	SetICCCM( display, window, argc, argv );
	SendText[ 0 ] = '\0';
	XFlush( display );
	MakeButtons( display, window, black, white, font->fid );
	MapWindow( display, window );
	while( QuitFlag == False )
	{
		EventLoop( display,
			window, AppGC, font,
			&width, &height );
		if( RefreshFlag == True )
		{
			Refresh( display, window, AppGC, font_height );
			RefreshFlag	= False;
		}
	}
	XFreeFont( display, font );
	XFreePixmap( display, icon );
	CloseDisplay( display, window, AppGC );
	exit( 0 );
}

SendData( display, window )
Display		*display;
Window		window;
{
	Window	send_window;
	
	send_window	= FindSendWindow( display, window );
	if( send_window != (Window) None )
	{
		SendString( display, send_window, SendText );
	}
	RefreshFlag = True;
	XFlush( display );
	XClearWindow( display, window );
}

ClearData( display, window )
Display		*display;
Window		window;
{
	SendText[ 0 ] = '\0';
	XClearWindow( display, window );
	RefreshFlag	= True;
}

EventLoop( display, window, gc, font, width, height )
Display		*display;
Window		window;
GC		gc;
XFontStruct	*font;
int		*width, *height;
{
	XEvent	event;
	KeySym	keysym;
	
	NextEvent( display, False, *width, *height, &event, &keysym );
	if( ButtonEvent( display, &event ) == True )
	{
		return( True );
	}
	switch( event.type )
	{
		case ClientMessage:
			if( IsDeleteWindow( display, &event ) == True )
			{
				QuitFlag = True;
			}
			break;
		case Expose:
			RefreshFlag	= True;
			break;
		case ConfigureNotify:
			*width = event.xconfigure.width;
			*height= event.xconfigure.height;
			break;
		case KeyPress:
			TextEdit( display, window, gc, font,
				5, 2 * BUTTON_HEIGHT, keysym,
				SendText, BUFSIZE );
			break;
	}
	return ( True );
}

QuitApplication( display, window )
Display	*display;
Window	window;
{
	QuitFlag = True;
}

MakeButtons( display, window, fore, back, font_id )
Display		*display;
Window		window;
unsigned long	fore, back;
Font		font_id;
{
	int	QuitApplication();
	int	SendData();
	int	ClearData();
	int	x;

	x = 1;
	CreateButton( display, window, x, 2, fore, back,
		font_id, "Quit", QuitApplication );
	x = BUTTON_WIDTH + 5;
	CreateButton( display, window, x, 2, fore, back,
		font_id, "Send", SendData );
	x += BUTTON_WIDTH + 5;
	CreateButton( display, window, x, 2, fore, back,
		font_id, "Clear", ClearData );
}

Refresh( display, window, gc, font_height )
Display		*display;
Window		window;
GC		gc;
int		font_height;
{
	XDrawImageString( display, window, gc,
		( 3 * (BUTTON_WIDTH + 5) ) + 5,
		BUTTON_HEIGHT - 5,
		MESSAGE, strlen( MESSAGE ) );
	XDrawImageString( display, window, gc,
		5, ( 2 * BUTTON_HEIGHT ),
		SendText, strlen( SendText ) );
	XFlush( display );
}

Window FindSendWindow( display, window )
Display	*display;
Window	window;
{
	int	screen;
	Window	rootwindow, send_window;
	Cursor	cursor;

	XDrawImageString( display, window, AppGC,
		( 3 * ( BUTTON_WIDTH + 5 ) ) + 5,
		BUTTON_HEIGHT - 5,
		PRESS_MESSAGE, strlen( PRESS_MESSAGE ) );
	XFlush( display );
	screen		= DefaultScreen( display );
	rootwindow	= RootWindow( display, screen );
	cursor		= XCreateFontCursor( display, XC_gumby );
	send_window	= PickWindow( display, rootwindow, cursor );
	XFreeCursor( display, cursor );
	return( send_window );
}

