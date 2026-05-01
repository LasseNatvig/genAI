# =============================================================================
# SDL3 Demo  —  Windows MSVC / NMake build
# =============================================================================
# Targets:
#   nmake setup       Download SDL3 3.4.6 dev libraries (requires PowerShell)
#   nmake             Build debug (default)
#   nmake debug       Debug build   -> bin\debug\demo.exe   (console, symbols)
#   nmake release     Release build -> bin\release\demo.exe (optimised, no console)
#   nmake test        Compile and run unit tests (no SDL required)
#   nmake clean       Remove all build outputs
# =============================================================================

SDL3_VERSION = 3.4.6
SDL3_ZIP     = SDL3-devel-$(SDL3_VERSION)-VC.zip
SDL3_URL     = https://github.com/libsdl-org/SDL/releases/download/release-$(SDL3_VERSION)/$(SDL3_ZIP)
SDL3_DIR     = SDL3
SDL3_INC     = $(SDL3_DIR)\include
SDL3_LIB     = $(SDL3_DIR)\lib\x64\SDL3.lib
SDL3_DLL     = $(SDL3_DIR)\lib\x64\SDL3.dll

CC      = cl
CFLAGS  = /nologo /W3 /I"$(SDL3_INC)" /I"src" /DSDL_MAIN_HANDLED

# Debug: no optimisation, full debug symbols, debug CRT
DBG_FLAGS = $(CFLAGS) /Od /Zi /D_DEBUG /MDd

# Release: full optimisation, release CRT
REL_FLAGS = $(CFLAGS) /O2 /DNDEBUG /MD

SRC         = src\main.c src\sprite.c src\audio.c
DEBUG_DIR   = bin\debug
RELEASE_DIR = bin\release
DEBUG_EXE   = $(DEBUG_DIR)\demo.exe
RELEASE_EXE = $(RELEASE_DIR)\demo.exe

# ==============================================================================
all: debug

# ------------------------------------------------------------------------------
# setup — download and extract SDL3 development libraries
# ------------------------------------------------------------------------------
setup:
	@echo Downloading SDL3 $(SDL3_VERSION) from GitHub...
	powershell -NoProfile -ExecutionPolicy Bypass -Command "Invoke-WebRequest -Uri '$(SDL3_URL)' -OutFile '$(SDL3_ZIP)'"
	@echo Extracting $(SDL3_ZIP)...
	powershell -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -Path '$(SDL3_ZIP)' -DestinationPath '.' -Force"
	powershell -NoProfile -ExecutionPolicy Bypass -Command "if (Test-Path '$(SDL3_DIR)') { Remove-Item -Recurse -Force '$(SDL3_DIR)' }; Rename-Item 'SDL3-$(SDL3_VERSION)' '$(SDL3_DIR)'"
	@del $(SDL3_ZIP)
	@echo SDL3 ready in $(SDL3_DIR)\

# ------------------------------------------------------------------------------
# debug — debug build with symbols and console window
# ------------------------------------------------------------------------------
debug:
	@if not exist $(DEBUG_DIR) mkdir $(DEBUG_DIR)
	$(CC) $(DBG_FLAGS) /Fo:$(DEBUG_DIR)\ $(SRC) /Fe:$(DEBUG_EXE) /Fd:$(DEBUG_DIR)\demo.pdb /link $(SDL3_LIB)
	@copy /Y "$(SDL3_DLL)" "$(DEBUG_DIR)\SDL3.dll" >nul
	@echo Debug build ready: $(DEBUG_EXE)

# ------------------------------------------------------------------------------
# release — optimised build, no console window
# ------------------------------------------------------------------------------
release:
	@if not exist $(RELEASE_DIR) mkdir $(RELEASE_DIR)
	$(CC) $(REL_FLAGS) /Fo:$(RELEASE_DIR)\ $(SRC) /Fe:$(RELEASE_EXE) /link $(SDL3_LIB) /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
	@copy /Y "$(SDL3_DLL)" "$(RELEASE_DIR)\SDL3.dll" >nul
	@echo Release build ready: $(RELEASE_EXE)

# ------------------------------------------------------------------------------
# test — compile and run unit tests (pure C, no SDL linkage)
# ------------------------------------------------------------------------------
test:
	$(CC) /nologo /W3 /I"src" /Fo:tests\ tests\test_sprite.c /Fe:tests\test_sprite.exe /link
	$(CC) /nologo /W3 /Fo:tests\ tests\test_audio.c /Fe:tests\test_audio.exe /link
	@echo.
	@echo --- Running test_sprite ---
	tests\test_sprite.exe
	@echo --- Running test_audio ---
	tests\test_audio.exe
	@echo.
	@echo Tests complete.

# ------------------------------------------------------------------------------
# clean — remove all build outputs
# ------------------------------------------------------------------------------
clean:
	@if exist bin                    rmdir /s /q bin
	@if exist tests\test_sprite.exe  del /q tests\test_sprite.exe
	@if exist tests\test_sprite.obj  del /q tests\test_sprite.obj
	@if exist tests\test_audio.exe   del /q tests\test_audio.exe
	@if exist tests\test_audio.obj   del /q tests\test_audio.obj
	@echo Clean done.
