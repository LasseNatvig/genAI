#!/usr/bin/env python
"""
Debug Snake Game for Raspberry Pi Sense HAT
Optimized version with reduced flicker and improved input handling
Starting with 3-pixel snake for easier debugging
"""

import sense_hat
import pygame
import time
from pygame.locals import *

class DebugSnakeGame:
    def __init__(self):
        # Initialize Sense HAT
        self.sense = sense_hat.SenseHat()
        self.sense.clear()

        # Debug settings
        self.debug_mode = True
        self.debug_messages = []

        # Game settings
        self.game_speed = 0.3  # seconds between moves (slower for debugging)
        self.running = True
        self.clock = pygame.time.Clock()
        self.frame_rate = 30  # frames per second

        # Snake: start with 3-pixel snake for easier debugging
        # Snake body grows in the direction opposite of movement
        self.snake = [(3, 4), (2, 4), (1, 4)]  # 3-pixel snake facing right
        self.direction = (1, 0)  # Initial direction: right
        self.next_direction = (1, 0)  # Buffer for next direction

        # Color settings
        self.snake_color = (0, 255, 0)  # Green
        self.head_color = (0, 200, 0)   # Slightly darker green for head
        self.background = (0, 0, 0)     # Black
        self.fruit_color = (255, 0, 0)  # Red for fruit (debugging aid)

        # Double buffering for flicker-free display
        self.pixels = [[self.background for _ in range(8)] for _ in range(8)]

        # Setup input modes
        self.last_input_time = 0
        self.input_cooldown = 0.1  # seconds between input processing

        if self.debug_mode:
            self.debug("Game initialized with 3-pixel snake")
            self.debug(f"Initial direction: {self.direction}")

    def debug(self, message: str) -> None:
        """
        Add a debug message to the debug log and print to terminal.

        Debug messages are stored in the debug log and printed to terminal
        for easier debugging during development.

        Args:
            message: The debug message to record and print
        """
        if not self.debug_mode:
            return

        # Store message in debug log
        self.debug_messages.append(message)
        if len(self.debug_messages) > 5:  # Keep last 5 messages
            self.debug_messages.pop(0)

        # Print to terminal
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        print(f"[{timestamp}] DEBUG: {message}")

    def move_up(self, event=None):
        """Change direction to up if not currently moving down"""
        current_time = time.time()
        if current_time - self.last_input_time > self.input_cooldown:
            if self.direction != (0, 1):
                self.next_direction = (0, -1)
                self.last_input_time = current_time
                if self.debug_mode:
                    self.debug("Move up")

    def move_down(self, event=None):
        """Change direction to down if not currently moving up"""
        current_time = time.time()
        if current_time - self.last_input_time > self.input_cooldown:
            if self.direction != (0, -1):
                self.next_direction = (0, 1)
                self.last_input_time = current_time
                if self.debug_mode:
                    self.debug("Move down")

    def move_left(self, event=None):
        """Change direction to left if not currently moving right"""
        current_time = time.time()
        if current_time - self.last_input_time > self.input_cooldown:
            if self.direction != (1, 0):
                self.next_direction = (-1, 0)
                self.last_input_time = current_time
                if self.debug_mode:
                    self.debug("Move left")

    def move_right(self, event=None):
        """Change direction to right if not currently moving left"""
        current_time = time.time()
        if current_time - self.last_input_time > self.input_cooldown:
            if self.direction != (-1, 0):
                self.next_direction = (1, 0)
                self.last_input_time = current_time
                if self.debug_mode:
                    self.debug("Move right")

    def update_direction(self):
        """Update direction from buffered input (synchronized with game loop)"""
        self.direction = self.next_direction
        if self.debug_mode:
            self.debug(f"Direction updated: {self.direction}")

    def update(self):
        """Update snake position"""
        # Update direction first (synchronized with game loop)
        self.update_direction()

        # Calculate new head position
        head_x, head_y = self.snake[0]
        dir_x, dir_y = self.direction
        new_x = (head_x + dir_x) % 8  # Wrap around screen
        new_y = (head_y + dir_y) % 8  # Wrap around screen
        new_head = (new_x, new_y)

        # Add new head and remove tail (keeping snake length constant)
        self.snake.insert(0, new_head)
        self.snake.pop()

        if self.debug_mode:
            self.debug(f"New head at ({new_x}, {new_y})")

    def draw(self):
        """Draw everything using double buffering technique"""
        # Reset pixel buffer
        self.pixels = [[self.background for _ in range(8)] for _ in range(8)]

        # Draw snake head
        head_x, head_y = self.snake[0]
        self.pixels[head_x][head_y] = self.head_color

        # Draw snake body
        for segment in self.snake[1:]:
            x, y = segment
            self.pixels[x][y] = self.snake_color

        # Apply buffered pixels to LED display
        for x in range(8):
            for y in range(8):
                self.sense.set_pixel(x, y, self.pixels[x][y])

    def handle_events(self):
        """Handle pygame events for keyboard support"""
        events = pygame.event.get()  # Get all events once
        for event in events:
            if event.type == QUIT:
                self.running = False
            elif event.type == KEYDOWN:
                if event.key == K_UP:
                    self.move_up()
                elif event.key == K_DOWN:
                    self.move_down()
                elif event.key == K_LEFT:
                    self.move_left()
                elif event.key == K_RIGHT:
                    self.move_right()
                elif event.key in (K_ESCAPE, K_q):
                    self.running = False
        # print("Event processed:", events)  # Debug: print all processed events

    def run(self):
        """Main game loop with proper frame timing"""
        # Initialize pygame for keyboard support
        pygame.init()
        pygame.display.set_mode((1, 1))  # Small window for event handling

        self.last_update_time = time.time()
        target_update_time = 1.0 / (1.0 / self.game_speed)  # About 3-4 updates per second

        try:
            while self.running:
                # Calculate delta time
                current_time = time.time()
                delta_time = current_time - self.last_update_time

                # Process input events
                self.handle_events()

                # Update game state at fixed intervals
                if delta_time >= self.game_speed:
                    self.update()
                    self.last_update_time = current_time

                # Draw continuously based on frame rate
                self.draw()
                self.clock.tick(self.frame_rate)

        except KeyboardInterrupt:
            print("\nGame stopped")
        finally:
            pygame.quit()
            self.sense.clear()
            if self.debug_mode:
                print("Game cleanup complete")
                for msg in self.debug_messages[-5:]:
                    print(f"DEBUG: {msg}")

if __name__ == "__main__":
    game = DebugSnakeGame()
    game.run()