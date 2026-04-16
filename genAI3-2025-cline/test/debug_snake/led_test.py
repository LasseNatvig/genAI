#!/usr/bin/env python
"""
LED Test for Raspberry Pi Sense HAT
Verifies LED hardware functionality without flicker
"""

import sense_hat
import time

class LEDTest:
    def __init__(self):
        # Initialize Sense HAT
        self.sense = sense_hat.SenseHat()
        self.sense.clear()
        self.running = True

        # Colors
        self.red = (255, 0, 0)
        self.green = (0, 255, 0)
        self.blue = (0, 0, 255)
        self.yellow = (255, 255, 0)
        self.black = (0, 0, 0)

        print("LED Test Started")
        print("Press Ctrl+C to stop")

    def test_simple(self):
        """Simple LED test - fill display with different colors"""
        try:
            print("Testing red...")
            self.sense.clear(self.red)
            time.sleep(1)

            print("Testing green...")
            self.sense.clear(self.green)
            time.sleep(1)

            print("Testing blue...")
            self.sense.clear(self.blue)
            time.sleep(1)

            print("Testing off...")
            self.sense.clear()

        except KeyboardInterrupt:
            pass
        except Exception as e:
            print(f"Error: {e}")
        finally:
            self.sense.clear()

    def test_pattern(self):
        """Test with patterns to check specific LEDs"""
        try:
            print("Testing checkerboard pattern...")
            for x in range(8):
                for y in range(8):
                    if (x + y) % 2 == 0:
                        self.sense.set_pixel(x, y, self.green)
                    else:
                        self.sense.set_pixel(x, y, self.black)
            time.sleep(2)

            print("Testing individual LEDs...")
            for x in range(8):
                for y in range(8):
                    self.sense.clear()
                    self.sense.set_pixel(x, y, self.yellow)
                    time.sleep(0.1)

        except KeyboardInterrupt:
            pass
        except Exception as e:
            print(f"Error: {e}")
        finally:
            self.sense.clear()

    def run(self):
        """Run all tests"""
        try:
            self.test_simple()
            time.sleep(0.5)
            self.test_pattern()
            time.sleep(0.5)
            print("LED Test Completed Successfully")
        finally:
            self.sense.clear()

if __name__ == "__main__":
    test = LEDTest()
    test.run()