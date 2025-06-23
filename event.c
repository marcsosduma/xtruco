/*
event.c
useful functions to handle
X11 events...
*/

#include "xbook.h"

Time LastEventTime = CurrentTime;

CheckEvent( display, want_exposes, width, height, event, keysym )
Display *display;
int 	want_exposes;
int 	width, height;
XEvent	*event;
KeySym	*keysym;
{
	int status;

	status = False;
	if( XPending( display ) > 0 )
	{
		XNextEvent( display, event );
		status = ParseEvent( event,
			want_exposes,
			width, height,
			keysym );
	}
	return ( status );
}

NextEvent( display, want_exposes, width, height, event, keysym )
Display	*display;
int	want_exposes;
int	width, height;
XEvent	*event;
KeySym	*keysym;
{
	int 	status;

	status = False;
	while( status == False )
	{
		XNextEvent( display, event );
		status = ParseEvent( event,
			want_exposes,
			width, height,
			keysym );
	}
}

ParseEvent( event, want_exposes, width, height, keysym )
XEvent	*event;
int	want_exposes;
int 	width, height;
KeySym	*keysym;
{
	int	status;

	status = False;
	switch( event->type )
	{
		case ButtonPress:
			LastEventTime = event->xbutton.time;
			status = True;
			break;
		case ClientMessage:
			status = True;
			break;
		case PropertyNotify:
			LastEventTime = event->xproperty.time;
			status = True;
			break;
		case SelectionClear:
			LastEventTime = event->xselectionclear.time;
			status = True;
			break;
		case SelectionNotify:
			LastEventTime = event->xselection.time;
			status = True;
			break;
		case SelectionRequest:
			LastEventTime = event->xselectionrequest.time;
			status = True;
			break;
		case ConfigureNotify:
			if( ( width != event->xconfigure.width ) ||
			    ( height != event->xconfigure.height ) )
			{
				status = True;
			}
			break;
		case Expose:
			status = True;
			if( want_exposes == False )
			{
				if( event->xexpose.count != 0 )
				{
					status = False;
				}
			}
			break;
		case KeyPress:
			LastEventTime = event->xkey.time;
			status = True;
			*keysym = Key2Keysym( event );
			break;
		case MappingNotify:
			XRefreshKeyboardMapping( (XMappingEvent *) event );
			break;
	}
	return ( status );
}

KeySym Key2Keysym( keyevent )
XKeyEvent	*keyevent;
{
	XComposeStatus	composestatus;
	KeySym		keysym;
	char		string[ 10 ];

	XLookupString( keyevent, string,
		9, &keysym, &composestatus );
	return ( keysym );
}

Time LastTimeStamp()
{
	return (LastEventTime);
}		
