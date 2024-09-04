# $Id$

PLATFORM = generic
PWD = `pwd`
FLAGS = PLATFORM=$(PLATFORM) PWD=$(PWD)

.PHONY: all format clean get-version

all: ./BASIC

./BASIC::
	$(MAKE) -C $@ $(FLAGS)

format:
	clang-format --verbose -i BASIC/basic.c

get-version:
	@grep "#define VERSION" BASIC/basic.c | sed -E 's/^#define VERSION "|"$$//g'

clean:
	$(MAKE) -C ./BASIC $(FLAGS) clean
	rm -f krakow.dsk
