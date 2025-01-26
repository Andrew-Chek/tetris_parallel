#include <SDL2/SDL_pixels.h>
#include <vector>

const int ROWS = 20;
const int COLS = 10;
const int CELL_SIZE = 30;
const int WINDOW_WIDTH = COLS * CELL_SIZE + 200;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;

const SDL_Color LOCKED_COLOR = {169, 169, 169};

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