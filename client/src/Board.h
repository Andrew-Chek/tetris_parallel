#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <sstream>
#include "Tetromino.h"

class Board {
public:
    const int ROWS = 20;
    const int COLS = 10;
    Board(); // Constructor

    // Accessors
    const std::vector<std::vector<int>>& getGrid() const;

    // Serialization and Deserialization
    std::string serializeBoard() const;
    void deserializeBoard(const std::string& serializedBoard);

    // Game Logic Methods
    int getDropDistance(const Tetromino& tetromino);
    bool checkCollision(const Tetromino& tetromino, int offsetX, int offsetY) const;
    void mergeTetromino(const Tetromino& tetromino);
    int clearLines();
    void clear(); // Clear the entire board

private:
    std::vector<std::vector<int>> grid; // 2D grid representing the game board
};

#endif // BOARD_H
