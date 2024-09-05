# $Id$

CC = cc65
LD = cl65
CFLAGS = -DPLATFORM_PET --static-locals -t pet -Os
LDFLAGS = -t pet -Os -o
LIBS =
EXEC = .prg
OBJ = .s
