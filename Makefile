# $Id$

PLATFORM = generic
PWD = `pwd`
FLAGS = PLATFORM=$(PLATFORM) PWD=$(PWD)

.PHONY: all clean

all: ./BASIC

./BASIC::
	$(MAKE) -C $@ $(FLAGS)

clean:
	$(MAKE) -C ./BASIC $(FLAGS) clean
