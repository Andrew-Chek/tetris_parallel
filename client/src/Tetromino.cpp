#include "Tetromino.h"

Tetromino::Tetromino(TetrominoType type, const std::vector<std::vector<int>>& shape, const Position& pos)
    : type(type), shape(shape), position(pos) {
}

const std::vector<std::vector<int>>& Tetromino::getShape() const {
    return shape;
}

const Position& Tetromino::getPosition() const {
    return position;
}

TetrominoType Tetromino::getType() const {
    return type;
}

void Tetromino::move(int dx, int dy) {
    position.x += dx;
    position.y += dy;
}

void Tetromino::rotate() {
    std::vector<std::vector<int>> rotated(shape[0].size(), std::vector<int>(shape.size()));
    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
            rotated[col][shape.size() - 1 - row] = shape[row][col];
        }
    }
    shape = rotated;
};