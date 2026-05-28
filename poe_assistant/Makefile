CC      = gcc
CFLAGS  = -g -O0 -Wall -Wextra
TARGET  = sensehat
SRC     = sensehat_i2c_ids.c

.PHONY: all clean test-keys sensehat 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)


clean:
	rm -f $(TARGET)
