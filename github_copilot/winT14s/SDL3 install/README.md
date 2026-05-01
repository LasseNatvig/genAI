nmak# SDL3 Demo — Windows / MSVC

A C demo showcasing SDL3 features: 2D rendering, bouncing sprite with delta-time physics, keyboard & mouse input, and 440 Hz sine-wave audio.

**Requires:** Visual Studio 2022 Build Tools (MSVC / nmake), PowerShell, Windows 11 x64.

## Quick Start

Open a **Developer Command Prompt for VS 2022** (or source `vcvars64.bat x64`), then:

nmake setup    # download SDL3 3.4.6 dev libs (one-time)
nmake          # build debug (default)
bin\debug\demo.exe

## nmake Targets

| Target | Description |
| `nmake setup` | Download & extract SDL3 3.4.6 VC libs into `SDL3\` |
| `nmake` / `nmake debug` | Debug build → `bin\debug\demo.exe` (`/Od /Zi`) |
| `nmake release` | Optimised build → `bin\release\demo.exe` (`/O2`, no console) |
| `nmake test` | Compile & run 15 headless unit tests (no SDL required) |
| `nmake clean` | Remove `bin\` and test binaries |

## Demo Controls

| Key / Action | Effect |
| `SPACE` | Toggle 440 Hz sine-wave audio |
| `R` | Randomise sprite colour |
| Left click | Teleport sprite to mouse position |
| `ESC` / close | Quit |

Audio status shown by the indicator square (top-right): **green** = on, **red** = off.

## Project Structure

src/
  main.c       event loop, rendering, input
  sprite.h/c   bouncing rect (delta-time physics)
  audio.h/c    sine-wave audio via SDL_AudioStream
tests/
  test_sprite.c  10 pure-C bounce physics tests
  test_audio.c    5 pure-C sine PCM math tests
SDL3/          SDL3 3.4.6 headers + libs (after setup)
bin/debug/     debug exe + SDL3.dll
bin/release/   release exe + SDL3.dll