# TDT4258 - 2026 - RPi4-1-setup Makefile 
###################################################################
CC=gcc
CFLAGS_DEBUG = -std=c11 -g -Wa,--noexecstack
SOURCES = main.c assembler.s 

# Builds debug version
test: $(SOURCES)
	$(CC) $(CFLAGS_DEBUG) -o test $(SOURCES)

# Clean  
clean:
	rm -f test
