/*
color.c
X11 routines to convert a color name to a color.
*/

#include "xbook.h"

unsigned long GetColor( display, name, colormap, default_color )
Display     *display;
char        name[];
Colormap    colormap;
unsigned long default_color;
{
     unsigned long   color;
     XColor          rgbcolor, hardwarecolor;
     int             status;

     color = default_color;
     status = XLookupColor( display, colormap,  name,
			   &rgbcolor, &hardwarecolor );
     if( status != 0 )
       {
	 status = XAllocColor( display, colormap, 
			      &hardwarecolor );
	 if( status != 0 )
	   {
	     color = hardwarecolor.pixel;
	   }
       }
     return (color);
   }
