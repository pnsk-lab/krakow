# $Id$

CC = cc65
LD = cl65
CFLAGS = -DPLATFORM_APPLE2 --static-locals -t apple2
LDFLAGS = -t apple2
LIBS =
EXEC =
OBJ = .s
TARGET = krakow.dsk
