#!/usr/bin/env python
"""
Input Test for Debug Snake Game
Tests keyboard input handling without game logic
"""

import pygame
import time
from pygame.locals import *

class InputTest:
    def __init__(self):
        # Initialize pygame
        pygame.init()
        pygame.display.set_mode((1, 1))  # Small display for event handling
        self.clock = pygame.time.Clock()

        # Input test state
        self.running = True
        self.last_key = None
        self.last_event = None
        self.key_count = 0

        print("Input Test Started")
        print("Press arrow keys to test input")
        print("Press ESC key OR Ctrl+C to stop test")
        print("=" * 50)

    def handle_events(self):
        """Handle pygame events"""
        for event in pygame.event.get():
            if event.type == QUIT:
                self.running = False
            elif event.type == KEYDOWN:
                self.last_event = event
                self.key_count += 1

                if event.key == K_UP:
                    self.last_key = 'UP'
                    print(f"↑ UP arrow   | Key: {event.key} | Unicode: {event.unicode}")
                elif event.key == K_DOWN:
                    self.last_key = 'DOWN'
                    print(f"↓ DOWN arrow | Key: {event.key} | Unicode: {event.unicode}")
                elif event.key == K_LEFT:
                    self.last_key = 'LEFT'
                    print(f"← LEFT arrow | Key: {event.key} | Unicode: {event.unicode}")
                elif event.key == K_RIGHT:
                    self.last_key = 'RIGHT'
                    print(f"→ RIGHT arrow| Key: {event.key} | Unicode: {event.unicode}")
                elif event.key == K_ESCAPE:
                    self.running = False
                    print("ESC pressed - Exiting")
                else:
                    print(f"? Unknown key  | Key: {event.key} | Unicode: {event.unicode}")

                # Show raw event details
                print(f"    Raw Event: {event}")
                print("-" * 50)

    def run(self):
        """Main test loop"""
        try:
            while self.running:
                self.handle_events()
                self.clock.tick(60)  # 60 FPS

                # Small delay to prevent CPU overload
                time.sleep(0.01)

        except KeyboardInterrupt:
            print("\nTest interrupted")
        finally:
            pygame.quit()
            print(f"\nTest Summary:")
            print(f"Total key presses: {self.key_count}")
            print(f"Last key pressed: {self.last_key}")
            print("Input Test Completed")

if __name__ == "__main__":
    test = InputTest()
    test.run()