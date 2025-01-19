#ifndef TETRIS_GAME_SERVICE_HPP
#define TETRIS_GAME_SERVICE_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <cstring>

#define BACKBOARD_BARRIER_SIZE 2

#define KEY_ARROW_UP 65
#define KEY_ARROW_DOWN 66
#define KEY_ARROW_RIGHT 67
#define KEY_ARROW_LEFT 68

#define BLOCK_I 100
#define BLOCK_T 101
#define BLOCK_L 102
#define BLOCK_L2 103
#define BLOCK_O 104
#define BLOCK_S 105
#define BLOCK_S2 106

typedef std::vector<std::vector<int>> TetrisBoard;

enum TetrisGameState {
    Running,
    Gameover
};

class TetrisGameService
{
public:
    TetrisGameService(TetrisBoard *block);
    TetrisGameState getGameState();
    void accept(std::queue<char> *keys);

private:
    TetrisGameState gameState;
    TetrisBoard backBoard;
    TetrisBoard *board;
    TetrisBoard extractBoardFromBackBoard();
    TetrisBoard generateBackBoard();
    void attachBoardToBackBoard();
    bool isArrowKey(char key, std::queue<char> *keys);
    bool isSpaceKey(char key);
    char getArrowKey(std::queue<char> *keys);
    void generateRandomBlock();
    void setBlock(int blockType, const int block[][4] );
    void updateBoard(int nextBlockTopLeftY, int nextBlockTopLeftX, int nextBlock[][4]);
    void doUpdateBoard(int nextBlockTopLeftY, int nextBlockTopLeftX, int nextBlock[][4]);
    bool isBlockCollide(int nextBlockTopLeftY, int nextBlockTopLeftX, int nextBlock[][4]);
    bool isBlockCollideBottom(int nextBlockTopLeftY, int nextBlock[][4]);
    void rotateBlock(int nextBlock[][4]);
    void breakFullLineFromBoard();
    void breakLine(int fullLineIdx);
    void updateGameState();
    int boardWidth;
    int boardHeight;
    int blockTopLeftY = 0;
    int blockTopLeftX = 0;
    int currentBlockType;
    int currentBlock[4][4] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0};
    const int blockI[4][4] = {
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0};
    const int blockT[4][4] = {
        0, 1, 0, 0,
        0, 1, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 0};
    const int blockL[4][4] = {
        0, 0, 1, 0,
        1, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0};
    const int blockL2[4][4] = {
        1, 0, 0, 0,
        1, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0};
    const int blockS[4][4] = {
        0, 1, 1, 0,
        1, 1, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0};
    const int blockS2[4][4] = {
        1, 1, 0, 0,
        0, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0};
    const int blockO[4][4] = {
        1, 1, 0, 0,
        1, 1, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0};
};

#endif
