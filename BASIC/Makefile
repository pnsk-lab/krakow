# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

TARGET ?= krakow$(EXEC)

.PHONY: all clean

all: $(TARGET)

krakow$(EXEC): krakow.o
	$(CC) $(LDFLAGS) -o $@ krakow.o $(LIBS)

krakow.hex: krakow.elf
	objcopy -O ihex -R $(SECTION) krakow.elf $@

krakow.o: basic.c
	$(CC) $(CFLAGS) -c -o $@ basic.c

clean:
	rm -f *.hex *.exe krakow *.o *.elf
