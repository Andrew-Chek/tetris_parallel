#ifndef TETROMINO_H
#define TETROMINO_H

#include <vector>
#include "Position.h"

enum class TetrominoType {
    I, O, T, S, Z, J, L
};

class Tetromino {
private:
    std::vector<std::vector<int>> shape;  // Shape of the Tetromino (matrix representation)
    Position position;                    // Position of the Tetromino on the board
    TetrominoType type;                   // Type of the Tetromino (e.g., I, O, T, etc.)

public:
    // Constructor
    Tetromino(TetrominoType type, const std::vector<std::vector<int>>& shape, const Position& pos);

    // Getters
    const std::vector<std::vector<int>>& getShape() const;
    const Position& getPosition() const;
    TetrominoType getType() const;

    // Setters
    void setPosition(const Position& pos);

    // Movement
    void move(int dx, int dy);

    // Rotate Tetromino 90 degrees clockwise
    void rotate();
};

#endif // TETROMINO_H
