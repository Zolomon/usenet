#
# Makefile that calls the Makefil in src/
#

.PHONY: all clean cleaner

all: 
	make -C src all

clean:
	make -C src clean

cleaner: clean
	make -C src cleaner