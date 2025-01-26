#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <SDL2/SDL_ttf.h>
#include "Board.h"

Board::Board() 
    : grid(ROWS, std::vector<int>(COLS, 0)) {
    // Additional initialization (if needed)
}

const std::vector<std::vector<int>>& Board::getGrid() const {
    return grid; // Return the grid by reference
}

std::string Board::serializeBoard() const {
    std::string state;
    for (const auto& row : grid) {
        for (int cell : row) {
            state += std::to_string(cell) + " ";
        }
        state += "\n";
    }
    return state;
}

void Board::deserializeBoard(const std::string& serializedBoard) {
    std::istringstream ss(serializedBoard);
    std::string line;
    int row = 0;

    // Parse the serialized board string line by line
    while (std::getline(ss, line) && row < ROWS) {
        std::istringstream rowStream(line);
        int col = 0;
        int value;
        while (rowStream >> value && col < COLS) {
            grid[row][col] = value;
            ++col;
        }
        ++row;
    }
}

int Board::getDropDistance(const Tetromino& tetromino) {
    const auto& shape = tetromino.getShape();
    const auto& pos = tetromino.getPosition();
    int dropDistance = 0;

    // Simulate dropping the tetromino one row at a time
    while (true) {
        bool collision = false;

        for (size_t row = 0; row < shape.size(); ++row) {
            for (size_t col = 0; col < shape[row].size(); ++col) {
                if (shape[row][col]) {
                    int newX = pos.x + col;
                    int newY = pos.y + dropDistance + row;

                    // Check if the new position is out of bounds or collides with the board
                    if (newY >= ROWS || newX < 0 || newX >= COLS || grid[newY][newX] != 0) {
                        collision = true;
                        break;
                    }
                }
            }
            if (collision) break;
        }

        if (collision) break;

        dropDistance++;
    }

    return dropDistance - 1; // Subtract 1 because the last move caused a collision
}

bool Board::checkCollision(const Tetromino& tetromino, int offsetX, int offsetY) const {
    const auto& shape = tetromino.getShape();
    const auto& pos = tetromino.getPosition();

    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col]) {
                int newX = pos.x + col + offsetX;
                int newY = pos.y + row + offsetY;

                if (newX < 0 || newX >= COLS || newY >= ROWS || (newY >= 0 && grid[newY][newX])) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Board::mergeTetromino(const Tetromino& tetromino) {
    const auto& shape = tetromino.getShape();
    const auto& pos = tetromino.getPosition();

    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col]) {
                grid[pos.y + row][pos.x + col] = shape[row][col];
            }
        }
    }
}

int Board::clearLines() {
    int count = 0;
    for (int row = ROWS - 1; row >= 0; --row) {
        bool fullLine = true;
        for (int col = 0; col < COLS; ++col) {
            if (!grid[row][col]) {
                fullLine = false;
                break;
            }
        }

        if (fullLine) {
            count++;
            for (int moveRow = row; moveRow > 0; --moveRow) {
                grid[moveRow] = grid[moveRow - 1];
            }
            grid[0] = std::vector<int>(COLS, 0);
            ++row; // Re-check this row
        }
    }
    return count;
}

void Board::clear() {
    // Iterate through each cell in the grid and set it to 0 (or another value that represents an empty cell)
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            grid[row][col] = 0;  // Assuming 0 represents an empty cell on the grid
        }
    }
}