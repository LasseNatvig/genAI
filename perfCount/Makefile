CC     = gcc
# CFLAGS = -O2 -Wall -Wextra -std=c11
CFLAGS = -std=c11 -O2 -Wa,--noexecstack 
# LDFLAGS = -lrt -lm
LDFLAGS = -lm
TARGET = perf_counters

$(TARGET): perf_counters_L1data.c functions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
