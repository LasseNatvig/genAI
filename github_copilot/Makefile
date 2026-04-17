CC      = gcc
CFLAGS  = -g -O0 -Wall -Wextra
TARGET  = hello
SRC     = hello.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
