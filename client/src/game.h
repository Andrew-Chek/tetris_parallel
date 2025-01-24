#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

const int ROWS = 20;
const int COLS = 10;
const int CELL_SIZE = 30;
const int WINDOW_WIDTH = COLS * CELL_SIZE;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;

const std::vector<std::vector<std::vector<int>>> TETROMINOS = {
    {{1, 1, 1, 1}}, // I-shape
    {{1, 1}, {1, 1}}, // O-shape
    {{0, 1, 0}, {1, 1, 1}}, // T-shape
    {{1, 1, 0}, {0, 1, 1}}, // S-shape
    {{0, 1, 1}, {1, 1, 0}}, // Z-shape
    {{1, 0, 0}, {1, 1, 1}}, // L-shape
    {{0, 0, 1}, {1, 1, 1}}  // J-shape
};

class Position {
public:
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

class Tetromino {
private:
    std::vector<std::vector<int>> shape;
    Position position;

public:
    Tetromino(const std::vector<std::vector<int>>& shape, const Position& pos);
    const std::vector<std::vector<int>>& getShape() const;
    const Position& getPosition() const;
    void setPosition(const Position& pos);
    void move(int dx, int dy);
    void rotate();
};

class Board {
private:
    std::vector<std::vector<int>> grid;

public:
    Board();
    const std::vector<std::vector<int>>& getGrid() const;
    std::string serializeBoard() const;
    void deserializeBoard(const std::string& serializedBoard);
    bool checkCollision(const Tetromino& tetromino, int offsetX, int offsetY) const;
    void mergeTetromino(const Tetromino& tetromino);
    void clearLines();
};

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Board board;
    Tetromino* currentTetromino;
    bool quit;
    bool gameOver;
    Uint32 lastTick;
    const Uint32 tickInterval = 500;

public:
    Game();
    ~Game();
    bool initialize();
    void spawnTetromino();
    void handleInput(const std::string& command);
    std::string serializeBoard() const;
    void render();
    void update();
    void run(const std::vector<std::string>& commands);
    void updateBoard(const std::string& serializedBoard);
    void cleanup();
};

#endif // GAME_H
