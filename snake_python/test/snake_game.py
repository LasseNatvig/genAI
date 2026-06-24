#!/usr/bin/env python
"""
Snake Game for Raspberry Pi Sense HAT
A 10-pixel long snake that moves around the 8x8 LED matrix
Controlled by arrow keys (works with both keyboard and Sense HAT joystick)
"""

import sense_hat
import pygame
from time import sleep
from pygame.locals import *

class SnakeGame:
    def __init__(self):
        # Initialize Sense HAT
        self.sense = sense_hat.SenseHat()
        self.sense.clear()

        # Game settings
        self.game_speed = 0.25  # seconds between moves
        self.running = True

        # Snake: list of (x, y) coordinates, head is first element
        # Start with 10-pixel snake on the left side moving right
        # All coordinates must be within 0-7 range
        self.snake = [(x, 4) for x in range(7, -1, -1)]  # First 8 pixels
        # Add 2 more pixels by wrapping to the right side
        self.snake.extend([(x, 4) for x in range(7, 5, -1)])
        self.direction = (1, 0)  # Initial direction: right

        # Color settings
        self.snake_color = (0, 255, 0)  # Green
        self.background = (0, 0, 0)     # Black

        # Set up event handlers
        self.sense.stick.direction_up = self.move_up
        self.sense.stick.direction_down = self.move_down
        self.sense.stick.direction_left = self.move_left
        self.sense.stick.direction_right = self.move_right

    def move_up(self, event):
        """Change direction to up if not currently moving down"""
        if self.direction != (0, 1):
            self.direction = (0, -1)

    def move_down(self, event):
        """Change direction to down if not currently moving up"""
        if self.direction != (0, -1):
            self.direction = (0, 1)

    def move_left(self, event):
        """Change direction to left if not currently moving right"""
        if self.direction != (1, 0):
            self.direction = (-1, 0)

    def move_right(self, event):
        """Change direction to right if not currently moving left"""
        if self.direction != (-1, 0):
            self.direction = (1, 0)

    def update(self):
        """Update snake position"""
        # Calculate new head position
        head_x, head_y = self.snake[0]
        dir_x, dir_y = self.direction
        new_x = (head_x + dir_x) % 8  # Wrap around screen
        new_y = (head_y + dir_y) % 8  # Wrap around screen
        new_head = (new_x, new_y)

        # Add new head and remove tail (keeping snake length constant at 10)
        self.snake.insert(0, new_head)
        self.snake.pop()

    def draw(self):
        """Draw the snake on the LED matrix"""
        # Clear the display
        self.sense.clear()

        # Draw snake
        for segment in self.snake:
            self.sense.set_pixel(segment[0], segment[1], self.snake_color)

    def run(self):
        """Main game loop"""
        # Initialize pygame for keyboard support
        pygame.init()

        try:
            while self.running:
                # Handle pygame events for keyboard support
                for event in pygame.event.get():
                    if event.type == QUIT:
                        self.running = False
                    elif event.type == KEYDOWN:
                        if event.key == K_UP:
                            self.move_up(None)
                        elif event.key == K_DOWN:
                            self.move_down(None)
                        elif event.key == K_LEFT:
                            self.move_left(None)
                        elif event.key == K_RIGHT:
                            self.move_right(None)

                self.update()
                self.draw()
                
                
        except KeyboardInterrupt:
            print("\nGame stopped")
        finally:
            pygame.quit()
            self.sense.clear()

if __name__ == "__main__":
    game = SnakeGame()
    game.run()