################################
# Makefile for Super Truco     #
# by Marcos M. Duma            #
# 	                       #
# Modify for your system       #
#                              #
################################

INCLUDES    = $(shell pkg-config --cflags x11)
LIBRARIES   = $(shell pkg-config --libs x11)

# Remove '-DIPC_OK' if you have problems with 'shared-memory' (IPC routines)
OPC         = -DIPC_OK -Wall -Wextra

CFILES      = xtruco.c bitmap.c cursor.c display.c event.c gc.c pixmap.c\
              topwind.c window.c color.c button.c font.c read.c loadbmp.c

CC          = gcc -O2 -g -Dlinux -DX11R6 $(OPC) 

PROG        = xtruco

RM          = /bin/rm

OBJS        = $(CFILES:.c=.o)

all: $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBRARIES)

clean:
	$(RM) -f $(OBJS)

allclean:
	$(RM) -f $(OBJS) $(PROG)
