# NMake Makefile for genAI-3 2026 winPC
# Use this with Microsoft's nmake tool
 
# Compiler settings
CC = clang

# Target names
TARGET_DBG = debug.exe
SRC = main.c 

# Mode-specific flags
CFLAGS_DBG = -Wall -Wextra -g -O0

# Targets
all: debug

debug: $(TARGET_DBG)

$(TARGET_DBG): $(SRC)
	$(CC) $(CFLAGS_DBG) $(INCLUDES) -o $(TARGET_DBG) $(SRC) $(LDFLAGS) $(LIBS)
	@echo Build debug version: $(TARGET_DBG)

clean:
	del /f /q $(TARGET_DBG) 2>nul
	del /f /q *.ilk 2>nul
	del /f /q *.pdb 2>nul