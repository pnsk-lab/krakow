# $Id$

PLATFORM = generic
PWD = `pwd`
FLAGS = PLATFORM=$(PLATFORM) PWD=$(PWD)

.PHONY: all clean get-version

all: ./BASIC

./BASIC::
	$(MAKE) -C $@ $(FLAGS)

get-version:
	@grep "#define VERSION" BASIC/basic.c | sed -E 's/^#define VERSION "|"$$//g'

clean:
	$(MAKE) -C ./BASIC $(FLAGS) clean
	rm -f krakow.dsk
