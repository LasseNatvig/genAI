# Makefile for sorting benchmark program
###################################################################
CC=gcc
CFLAGS_DEBUG = -std=c11 -g -Wa,--noexecstack
CFLAGS_OPT = -std=c11 -O2 -Wa,--noexecstack
SOURCES = main.c

# Builds debug version
debug: $(SOURCES)
	$(CC) $(CFLAGS_DEBUG) -o test $(SOURCES)

# Builds optimized version (for benchmarking)
test: $(SOURCES)
	$(CC) $(CFLAGS_OPT) -o test $(SOURCES)

# Clean
clean:
	rm -f test