# Debug Snake Game

This directory contains debug versions of the snake game and test scripts to help identify and fix issues.

## Issues Being Addressed

1. **LED Flickering**: Fixed using double buffering technique
2. **Movement Problems**: Starting with a shorter 3-pixel snake for easier testing
3. **Keyboard Input Issues**: Improved input handling with proper buffering

## Test Scripts

### 1. LED Test (`led_test.py`)
Tests hardware LED functionality to ensure the Sense HAT is working properly.

**Usage:**
```bash
python led_test.py
```

This test will:
- Fill display with red, green, blue colors
- Show checkerboard pattern
- Light individual LEDs sequentially

### 2. Input Test (`input_test.py`)
Tests keyboard input handling to verify arrow keys are detected correctly.

**Usage:**
```bash
python input_test.py
```

This test will:
- Show raw keyboard events
- Display which arrow keys are being pressed
- Show unicode values and event details

### 3. Debug Snake Game (`snake_debug.py`)
Modified snake game with debugging features.

**Usage:**
```bash
python snake_debug.py
```

Key improvements:
- 3-pixel snake for easier debugging
- Double buffering to reduce LED flickering
- Input cooldown to prevent input overload
- Direction buffering to synchronize input with game logic
- Debug messages on the LED display
- Proper frame timing

## Debugging Workflow

1. **Test LED Hardware**: Run `led_test.py` to ensure LEDs work properly
2. **Test Keyboard Input**: Run `input_test.py` to verify keyboard detection
3. **Test Game**: Run `snake_debug.py` to test all functionality
4. **Observe Behavior**: Use the 3-pixel snake to easily see movement patterns
5. **Check Debug Output**: Look at terminal output for debugging information

## Arrow Key Issues

If arrow keys show as escape sequences (e.g., ^[[A^[[B), this indicates:
- The terminal is interpreting arrow keys in "application mode"
- PyGame isn't capturing proper key events

Try these solutions:
1. Run the game from a standard terminal (not a Python IDE)
2. Ensure PyGame is properly initialized
3. Try different terminal emulators if available

## Additional Debugging Commands

Run all tests sequentially:
```bash
python run_tests.py