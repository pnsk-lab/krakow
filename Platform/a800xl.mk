# $Id$

CC = cc65
LD = cl65
CFLAGS = -DPLATFORM_A800XL --static-locals -t atarixl
LDFLAGS = -t atarixl
LIBS =
EXEC = .rom
OBJ = .s
