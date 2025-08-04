/*
display.c
X11 C routines to open and close display connections

routines:
Display *SetUpDisplay( argc, argv, &screen )
Display *OpenDisplay( display_name, &screen )
CheckDisplayName( argc, argv, display_name )
CloseDisplay( disp, window, gc )
QuitX(display, error_message, error_file )
*/

#include "xbook.h"

void exit(int );

int CheckDisplayName( int argc, char *argv[], char display_name[])
{
	int counter;
	
	display_name[ 0 ] = '\0';
	counter = 1;
	while( counter < argc )
	{
		if( strncmp( argv[ counter ], "-display", 8 )==0 )
		{
			counter++;
			if( counter < argc )
			{
				(void) strcpy( display_name, argv[ counter ] );
			}
			else
			{
				(void) fprintf( stderr,
					"Error: usage is -display DisplayName\n" );
			}
		}
		counter++;
	}
	return 0;
}

Display *SetUpDisplay(int argc, char *argv[], int *screen)
{
	char display_name[ 120 ];
	Display *display;

	CheckDisplayName( argc, argv, display_name );
	display = OpenDisplay( display_name, screen );
	if( display == (Display *) NULL )
	{
		(void) fprintf(stderr, "Error could not open X display to %s.\n",
				XDisplayName( display_name ) );
		exit( 1 );
	}
	return ( display );
}

Display *OpenDisplay( display_name, screen )
char display_name[];
int *screen;
{
	Display *display;
	
	display= XOpenDisplay( display_name );
	if( display != (Display *) NULL)
	{ 
		*screen = DefaultScreen( display );
	}
	return ( display );
}

int CloseDisplay( Display *display, Window window, GC gc)
{
	XFreeGC( display, gc);
	XDestroySubwindows( display, window );
	XDestroyWindow( display, window );
	XFlush( display );
	XCloseDisplay( display );
	return 0;
}

int QuitX( Display *display, char error_message[], char error_file[])
{
	(void) fprintf(stderr, "ERROR: %s%s\n",
		error_message,
		error_file );

	XCloseDisplay( display );
	exit(1);
}

