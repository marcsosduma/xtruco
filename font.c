/*
font.c
X11 routines for loading a font
*/

#include "xbook.h"

XFontStruct *LoadFont( display, gc, argc, argv, default_name )
Display 	*display;
GC		gc;
int		argc;
char		*argv[];
char		default_name[];
{
	XFontStruct	*font;
	char		name[ BUFSIZE + 1 ];

	(void) strcpy( name, default_name );
	CheckFontName( argc, argv, name );
	font = XLoadQueryFont( display, name );
	if( font == (XFontStruct *) NULL )
	{
		font = XLoadQueryFont( display, default_name );
		if( font == (XFontStruct *) NULL )
		{
			font = XLoadQueryFont( display, "fixed" );
		}
	}
	if( font != (XFontStruct *) NULL )
	{
		XSetFont( display, gc, font->fid );
	}
	else
	{
		QuitX( display, "Error: could not load font ", 
			name );
	}
	return ( font );
}

CheckFontName( argc, argv, fontname )
int	argc;
char	*argv[];
char	fontname[];
{
	int counter;
	
	counter = 1;
	while( counter < argc )
	{
		if( ( strncmp( argv[ counter ], "-font", 5 ) == 0 ) ||
		  ( strncmp( argv[ counter ], "-fn", 3 ) == 0 ) )
		{
			counter++;
			if( counter < argc )
			{
				(void) strcpy( fontname, argv[counter] );
			}
			else
			{
				(void) fprintf( stderr,
				  "Error: the usage mode is -font FontName\n" );
			}
		}
		counter++;
	}
}

