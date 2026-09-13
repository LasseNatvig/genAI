CC     = gcc
# CFLAGS = -O2 -Wall -Wextra -std=c11
CFLAGS = -std=c11 -O2 -g -Wa,--noexecstack 
# CFLAGS = -std=c11 -g -Wa,--noexecstack # for debug  
# LDFLAGS = -lrt -lm
LDFLAGS = -lm
TARGET = perf_counters

$(TARGET): perf_counters_L1data.c sorting.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

plot:
	python3 plot_perf.py

run:
	./$(TARGET) 30000 3
	@LATEST_CSV=$$(ls -t res/*.csv | head -1); \
	 python3 plot_perf.py "$$LATEST_CSV"

.PHONY: clean plot run
