# $Id$

CC = cc65
LD = cl65
CFLAGS = -DPLATFORM_APPLE2 --static-locals -t apple2 -Os
LDFLAGS = -t apple2 -Os
LIBS =
EXEC =
OBJ = .s
TARGET = krakow.dsk
