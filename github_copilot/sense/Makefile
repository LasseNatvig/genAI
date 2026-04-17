CC      = gcc
CFLAGS  = -g -O0 -Wall -Wextra
TARGET  = sense
SRC     = sense.c

.PHONY: all clean test-keys test-led

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test-keys: $(TARGET)
	./$(TARGET) --test-keys

test-led: $(TARGET)
	./$(TARGET) --test-led

clean:
	rm -f $(TARGET)
