CC = cl
CFLAGS = /O2 /W3   # /O2: optimize for speed, /W3: warning level 3

all: cache_demo.exe

cache_demo.exe: cache_demo.c
	$(CC) $(CFLAGS) cache_demo.c /Fe:cache_demo.exe

clean:
	del /Q cache_demo.exe cache_demo.obj 2>nul

run: cache_demo.exe
	cache_demo.exe > results.txt

plot: results.txt
	python plot_results.py

full: clean all run plot
