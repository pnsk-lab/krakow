# $Id$

CC = cc65
LD = cl65
CFLAGS = -DPLATFORM_C64 --static-locals
LDFLAGS = --config $(PWD)/c64rom.cfg
LIBS = c64rom.s
EXEC = .80
OBJ = .s
