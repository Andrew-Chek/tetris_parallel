#include <SDL2/SDL_pixels.h>
#include <vector>

// Constants
const int ROWS = 20;
const int COLS = 10;
const int CELL_SIZE = 30;
const int WINDOW_WIDTH = COLS * CELL_SIZE + 200;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;
const SDL_Color I_COLOR = {0, 255, 255}; // Cyan
const SDL_Color O_COLOR = {255, 255, 0}; // Yellow
const SDL_Color T_COLOR = {128, 0, 128}; // Purple
const SDL_Color S_COLOR = {0, 255, 0}; // Green
const SDL_Color Z_COLOR = {255, 0, 0}; // Red
const SDL_Color J_COLOR = {0, 0, 255}; // Blue
const SDL_Color L_COLOR = {255, 165, 0}; // Orange

// Define a color for locked figures (light gray)
const SDL_Color LOCKED_COLOR = {169, 169, 169}; // Light gray

const std::vector<std::vector<std::vector<int>>> TETROMINOS = {
    {  // I shape
        {{1, 1, 1, 1}}
    },
    {  // O shape
        {{1, 1},
         {1, 1}}
    },
    {  // T shape
        {{0, 1, 0},
         {1, 1, 1}}
    },
    {  // S shape
        {{0, 1, 1},
         {1, 1, 0}}
    },
    {  // Z shape
        {{1, 1, 0},
         {0, 1, 1}}
    },
    {  // J shape
        {{1, 0, 0},
         {1, 1, 1}}
    },
    {  // L shape
        {{0, 0, 1},
         {1, 1, 1}}
    }
};