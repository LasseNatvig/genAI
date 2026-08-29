# Makefile for Quicksort comparison experiment

CC=gcc
CFLAGS_DEBUG = -std=c11 -g -Wa,--noexecstack
CFLAGS_OPT = -std=c11 -O2 -Wa,--noexecstack
SOURCES = main_comparison.c
TARGET = quicksort_comparison

# Builds debug version
debug: $(SOURCES)
	$(CC) $(CFLAGS_DEBUG) -o $(TARGET) $(SOURCES)

# Builds optimized version (for benchmarking)
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS_OPT) -o $(TARGET) $(SOURCES)

# Clean
clean:
	rm -f $(TARGET)

.PHONY: debug clean