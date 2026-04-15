import pygame
import sys

# Constants
GRID_SIZE = 8
SQUARE_SIZE = 10
SCREEN_SIZE = GRID_SIZE * SQUARE_SIZE
COLORS = {
    'BLUE': (0, 0, 255),
    'RED': (255, 0, 0),
    'WHITE': (255, 255, 255)
}

def initialize_game():
    """Initialize the game board with one red square at top-left corner"""
    grid = [[COLORS['BLUE'] for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]
    grid[0][0] = COLORS['RED']  # Starting position (top-left corner)
    player_pos = [0, 0]  # [row, column]
    return grid, player_pos

def draw_grid(screen, grid):
    """Draw the game grid on the screen"""
    for row in range(GRID_SIZE):
        for col in range(GRID_SIZE):
            rect = pygame.Rect(
                col * SQUARE_SIZE,
                row * SQUARE_SIZE,
                SQUARE_SIZE,
                SQUARE_SIZE
            )
            pygame.draw.rect(screen, grid[row][col], rect)

def move_player(grid, player_pos, direction):
    """Move the player in the specified direction if possible"""
    old_row, old_col = player_pos
    new_row, new_col = old_row, old_col

    # Calculate new position
    if direction == 'UP' and old_row > 0:
        new_row = old_row - 1
    elif direction == 'DOWN' and old_row < GRID_SIZE - 1:
        new_row = old_row + 1
    elif direction == 'LEFT' and old_col > 0:
        new_col = old_col - 1
    elif direction == 'RIGHT' and old_col < GRID_SIZE - 1:
        new_col = old_col + 1

    # Only move if position changed
    if [new_row, new_col] != [old_row, old_col]:
        # Mark old position as white (visited)
        grid[old_row][old_col] = COLORS['WHITE']

        # Move player to new position
        grid[new_row][new_col] = COLORS['RED']
        player_pos[:] = [new_row, new_col]

def check_win_condition(grid):
    """Check if all squares have been converted to white"""
    for row in range(GRID_SIZE):
        for col in range(GRID_SIZE):
            if grid[row][col] != COLORS['WHITE']:
                return False
    return True

def main():
    """Main game function"""
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_SIZE, SCREEN_SIZE))
    pygame.display.set_caption('Grid Game')

    grid, player_pos = initialize_game()
    clock = pygame.time.Clock()
    game_over = False

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            if not game_over and event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP:
                    move_player(grid, player_pos, 'UP')
                elif event.key == pygame.K_DOWN:
                    move_player(grid, player_pos, 'DOWN')
                elif event.key == pygame.K_LEFT:
                    move_player(grid, player_pos, 'LEFT')
                elif event.key == pygame.K_RIGHT:
                    move_player(grid, player_pos, 'RIGHT')

                # Check if player won
                if check_win_condition(grid):
                    game_over = True
                    print("Congratulations! You've converted all squares to white!")

        # Draw everything
        screen.fill((0, 0, 0))  # Clear screen with black
        draw_grid(screen, grid)
        pygame.display.flip()

        # Cap the frame rate
        clock.tick(60)

if __name__ == "__main__":
    main()