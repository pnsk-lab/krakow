# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

TARGET ?= krakow$(EXEC)

.PHONY: all clean

all: $(TARGET)

krakow$(EXEC): krakow$(OBJ)
	$(LD) $(LDFLAGS) $@ krakow$(OBJ) $(LIBS) || ( rm -f krakow$(EXEC) ; exit 1 )

krakow.msx.rom: krakow.msx.bin krakow.msx.ihx
	hex2bin -e rom krakow.msx.ihx
	dd if=krakow.msx.bin of=krakow.msx.rom obs=1 seek=256 conv=notrunc

krakow.msx.bin: krakow$(EXEC)
	hex2bin -e msx.bin krakow$(EXEC)

krakow.msx.ihx: msx.rel
	sdldz80 -i $@ msx.rel

msx.rel: msx.s
	sdasz80 -o $@ msx.s

krakow.hex: krakow.elf
	objcopy -O ihex -R $(SECTION) krakow.elf $@

krakow.dsk: krakow$(EXEC) $(PWD)/master.dsk $(PWD)/ac.jar
	cp $(PWD)/master.dsk krakow.dsk
	java -jar $(PWD)/ac.jar -as krakow.dsk krakow < krakow$(EXEC)

krakow$(OBJ): basic.c
	$(CC) $(CFLAGS) -o $@ basic.c

clean:
	rm -f *.hex *.exe krakow *.o *.elf krakow.s *.80 *.rom *.dsk *.prg *.sym *.lst *.rel *.ihx *.asm *.bin
