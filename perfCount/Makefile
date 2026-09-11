CC     = gcc
CFLAGS = -O0 -Wall -Wextra -std=c11
TARGET = perf_counters

$(TARGET): perf_counters_L1data.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: clean
