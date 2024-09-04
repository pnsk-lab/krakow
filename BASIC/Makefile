# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

TARGET ?= krakow$(EXEC)

.PHONY: all clean

all: $(TARGET)

krakow$(EXEC): krakow$(OBJ)
	$(LD) $(LDFLAGS) -o $@ krakow$(OBJ) $(LIBS)

krakow.hex: krakow.elf
	objcopy -O ihex -R $(SECTION) krakow.elf $@

krakow.dsk: krakow$(EXEC) $(PWD)/master.dsk $(PWD)/ac.jar
	cp $(PWD)/master.dsk $(PWD)/krakow.dsk
	java -jar $(PWD)/ac.jar -as $(PWD)/krakow.dsk krakow < krakow$(EXEC)

krakow$(OBJ): basic.c
	$(CC) $(CFLAGS) -o $@ basic.c

clean:
	rm -f *.hex *.exe krakow *.o *.elf krakow.s *.80 *.rom
