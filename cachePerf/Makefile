CC = gcc
CFLAGS = -O2 -Wall

all: cache_demo

cache_demo: cache_demo.c
	$(CC) $(CFLAGS) -o cache_demo cache_demo.c -lm

clean:
	rm -f cache_demo

run: cache_demo
	./cache_demo > results.txt

plot: results.txt
	python3 plot_results.py

full: clean all run plot

.PHONY: all clean run plot full