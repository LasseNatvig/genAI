CC     = gcc
CFLAGS = -O0 -Wall -Wextra -std=c11
LDFLAGS = -lrt
TARGET = perf_counters

$(TARGET): perf_counters_L1data.c functions.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
