# $Id$

CC = cc65
LD = cl65
CFLAGS = -DPLATFORM_A800XL --static-locals -t atarixl -Os
LDFLAGS = -t atarixl -Os -o
LIBS =
EXEC =
OBJ = .s
