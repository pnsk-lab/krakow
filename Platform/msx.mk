# $Id$

CC = sdcc
LD = sdldz80
CFLAGS = -DPLATFORM_MSX --code-loc 0x4100 -mz80 -c
LDFLAGS = -b _CODE=0x4100 -b _DATA=0x8000 -k $$SDCC/lib/z80 -i
LIBS = -l z80.lib
TARGET = krakow.msx.rom
EXEC = .ihx
OBJ = .rel
