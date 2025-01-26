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
    Board();

    const std::vector<std::vector<int>>& getGrid() const;

    std::string serializeBoard() const;
    bool checkCollision(const Tetromino& tetromino, int offsetX, int offsetY) const;
    void mergeTetromino(const Tetromino& tetromino);
    int clearLines();
    void clear();

private:
    std::vector<std::vector<int>> grid;
};

#endif
