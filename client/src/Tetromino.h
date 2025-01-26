#ifndef TETROMINO_H
#define TETROMINO_H

#include <vector>
#include "Position.h"

enum class TetrominoType {
    I, O, T, S, Z, J, L
};

class Tetromino {
private:
    std::vector<std::vector<int>> shape;
    Position position;
    TetrominoType type;

public:
    Tetromino(TetrominoType type, const std::vector<std::vector<int>>& shape, const Position& pos);

    const std::vector<std::vector<int>>& getShape() const;
    const Position& getPosition() const;
    TetrominoType getType() const;

    void move(int dx, int dy);

    void rotate();
};

#endif
