/*
read.c
X11 routines to handle user inputs
through invisible "push-button" inputs.
*/
#include "xbook.h"

#define	INV_MAX_BUTTONS 5	


struct {
	Display	*display;
	Window	w;
	int x, y, largura, altura;
	int (*function) ();
	} InvButtons[ INV_MAX_BUTTONS + 1 ];

int		inv_used = 0;
int		qual=1;

Bool InvButton(Display *display, Window window, int x, int y, int c, int a,
               int (*function)(Display *, Window, int))
{
	if( inv_used < ( INV_MAX_BUTTONS - 1) )
	{
 		InvButtons[ inv_used ].display	= display;
		InvButtons[ inv_used ].w		= window;
		InvButtons[ inv_used ].x		= x;
		InvButtons[ inv_used ].y		= y;
		InvButtons[ inv_used ].largura	= c;
		InvButtons[ inv_used ].altura	= a;
		InvButtons[ inv_used ].function	= function;
 		inv_used++;
		return( True );
	}
	return( False);
}


int InvFind( Display *display, Window window, int p1, int p2)
{
	int which_button;
	int x, y, c, a;

	for( which_button = 0; which_button < inv_used; which_button++ )
	{
		if( ( window == InvButtons[ which_button ].w ) &&
		  ( display == InvButtons[ which_button ].display ) )
		{
			x= InvButtons[ which_button ].x;
			y= InvButtons[ which_button ].y;
			c= InvButtons[ which_button ].largura;
			a= InvButtons[ which_button ].altura;
			if((p1>=x) && (p1<=(x+c)) && (p2>=y) && (p2<=(y+a))) 
				return( which_button );
		}
	}
	return ( -1 );
}

int InvEvent( Display *display,  XEvent	*event)
{
	int	which_button;
	
	switch( event->type )
	{
		case ButtonPress:
			if(event->xbutton.button ==1) qual=1; else qual=10;
			which_button = InvFind( display,
			    event->xbutton.window, 
			    event->xbutton.x,
			    event->xbutton.y );
			if( which_button >= 0 )
			{
 				(InvButtons[ which_button ].function)(display,
				  InvButtons[ which_button ].w, qual );
				return( True );
			}
			break;
	}
	return( False );
}
