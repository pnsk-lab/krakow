# $Id$

CC = cc
LD = cc
CFLAGS = -std=c99 -DPLATFORM_X11 -c -I /usr/X11R7/include
LDFLAGS = -L /usr/X11R7/lib -Wl,-R/usr/X11R7/lib -o
LIBS = -lX11 -lpthread
EXEC =
OBJ = .o
EXTOBJ = x11$(OBJ)
