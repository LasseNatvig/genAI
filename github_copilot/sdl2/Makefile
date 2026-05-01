CC      := gcc
SRC     := src/main.c
CFLAGS_COMMON := -Wall -Wextra -std=c11 $(shell pkg-config --cflags sdl2)
LIBS    := $(shell pkg-config --libs sdl2)

# ── release ───────────────────────────────────────────────────────────
RELEASE_DIR  := build/release
RELEASE_BIN  := $(RELEASE_DIR)/demo
RELEASE_FLAGS := -O2

# ── debug ─────────────────────────────────────────────────────────────
DEBUG_DIR    := build/debug
DEBUG_BIN    := $(DEBUG_DIR)/demo
DEBUG_FLAGS  := -O0 -g3 -fsanitize=undefined

# ── targets ───────────────────────────────────────────────────────────
.PHONY: all release debug run run-debug clean

all: release

release: $(RELEASE_BIN)

$(RELEASE_BIN): $(SRC) | $(RELEASE_DIR)
	$(CC) $(CFLAGS_COMMON) $(RELEASE_FLAGS) $< -o $@ $(LIBS)
	@echo "Release build: $@"

debug: $(DEBUG_BIN)

$(DEBUG_BIN): $(SRC) | $(DEBUG_DIR)
	$(CC) $(CFLAGS_COMMON) $(DEBUG_FLAGS) $< -o $@ $(LIBS)
	@echo "Debug build:   $@"

$(RELEASE_DIR) $(DEBUG_DIR):
	mkdir -p $@

run: release
	./$(RELEASE_BIN)

run-debug: debug
	./$(DEBUG_BIN)

clean:
	rm -rf build/
	@echo "Cleaned build directory."
