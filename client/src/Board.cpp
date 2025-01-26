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
}

const std::vector<std::vector<int>>& Board::getGrid() const {
    return grid;
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
            ++row;
        }
    }
    return count;
}

void Board::clear() {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            grid[row][col] = 0;
        }
    }
}