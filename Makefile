# $Id$

PLATFORM = generic
PWD = `pwd`
FLAGS = PLATFORM=$(PLATFORM) PWD=$(PWD)

.PHONY: all format clean

all: ./BASIC

./BASIC::
	$(MAKE) -C $@ $(FLAGS)

format:
	clang-format --verbose -i BASIC/basic.c

clean:
	$(MAKE) -C ./BASIC $(FLAGS) clean
	rm -f krakow.dsk
