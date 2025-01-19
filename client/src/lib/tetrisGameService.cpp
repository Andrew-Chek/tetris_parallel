#include "../../include/lib/tetrisGameService.hpp"

// https://gamedevelopment.tutsplus.com/tutorials/implementing-tetris-collision-detection--gamedev-852

void debugPrintBoard(TetrisBoard tempBoard)
{
    for (int i = 0; i < (int)(tempBoard).size(); ++i)
    {
        for (int j = 0; j < (int)(tempBoard)[0].size(); ++j)
        {
            std::cout << tempBoard[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

TetrisGameService::TetrisGameService(TetrisBoard *board)
{
    srand((unsigned int)time(NULL));
    gameState = TetrisGameState::Running;
    this->board = board;
    boardHeight = (*board).size();
    boardWidth = (*board)[0].size();
    backBoard = generateBackBoard();
    generateRandomBlock();
    int nextBlockTopLeftX = boardWidth / 2 - 1;
    int nextBlockTopLeftY = 0;
    updateBoard(nextBlockTopLeftY, nextBlockTopLeftX, currentBlock);
}

TetrisGameState TetrisGameService::getGameState() {
    return gameState;
}

void TetrisGameService::accept(std::queue<char> *keys)
{
    while (!keys->empty())
    {
        if (gameState == TetrisGameState::Gameover)
        {
            break;
        }

        char key = keys->front();
        keys->pop();
        int nextBlockTopLeftY = blockTopLeftY;
        int nextBlockTopLeftX = blockTopLeftX;
        int nextBlock[4][4];
        memcpy(nextBlock, currentBlock, 4 * 4 * sizeof(int));
        if (key == ' ')
        {
            while (!isBlockCollide(nextBlockTopLeftY, nextBlockTopLeftX, nextBlock))
            {
                nextBlockTopLeftY += 1;
            }
            doUpdateBoard(nextBlockTopLeftY - 1, nextBlockTopLeftX, nextBlock);
            updateGameState();
        }
        else if (isArrowKey(key, keys))
        {
            key = getArrowKey(keys);
            switch (key)
            {
            case KEY_ARROW_UP:
                rotateBlock(nextBlock);
                break;
            case KEY_ARROW_DOWN:
                nextBlockTopLeftY += 1;
                break;
            case KEY_ARROW_RIGHT:
                nextBlockTopLeftX += 1;
                break;
            case KEY_ARROW_LEFT:
                nextBlockTopLeftX -= 1;
                break;
            }
            updateBoard(nextBlockTopLeftY, nextBlockTopLeftX, nextBlock);
        }
    }
}

void TetrisGameService::updateGameState()
{
    breakFullLineFromBoard();
    attachBoardToBackBoard();
    generateRandomBlock();
    int nextBlockTopLeftX = boardWidth / 2 - 1;
    int nextBlockTopLeftY = 0;
    if (isBlockCollide(nextBlockTopLeftY, nextBlockTopLeftX, currentBlock))
    {
        doUpdateBoard(nextBlockTopLeftY, nextBlockTopLeftX, currentBlock);
        gameState = TetrisGameState::Gameover;
        // std::cout << "게임 종료\n";
    }
    else
    {
        updateBoard(nextBlockTopLeftY, nextBlockTopLeftX, currentBlock);
    }
}

TetrisBoard TetrisGameService::generateBackBoard()
{
    TetrisBoard tempBoard = TetrisBoard(boardHeight + BACKBOARD_BARRIER_SIZE, std::vector<int>(boardWidth + BACKBOARD_BARRIER_SIZE * 2, 1));
    for (int i = 0; i < boardHeight; ++i)
    {
        for (int j = 0; j < boardWidth; ++j)
        {
            tempBoard[i][BACKBOARD_BARRIER_SIZE + j] = 0;
        }
    }
    return tempBoard;
}

void TetrisGameService::attachBoardToBackBoard()
{
    for (int i = 0; i < boardHeight; ++i)
    {
        for (int j = 0; j < boardWidth; ++j)
        {
            backBoard[i][BACKBOARD_BARRIER_SIZE + j] = (*board)[i][j];
        }
    }
}

TetrisBoard TetrisGameService::extractBoardFromBackBoard()
{
    TetrisBoard tempBoard = TetrisBoard(boardHeight, std::vector<int>(boardWidth, 0));
    for (int i = 0; i < boardHeight; ++i)
    {
        for (int j = 0; j < boardWidth; ++j)
        {
            tempBoard[i][j] = backBoard[i][BACKBOARD_BARRIER_SIZE + j];
        }
    }
    return tempBoard;
}

bool TetrisGameService::isArrowKey(char key, std::queue<char> *keys)
{
    return key == '\033' && keys->size() >= 2;
}

char TetrisGameService::getArrowKey(std::queue<char> *keys)
{
    keys->pop();
    char key = keys->front();
    keys->pop();
    return key;
}

void TetrisGameService::generateRandomBlock()
{
    int block = rand() % 7;
    switch (block)
    {
    case 0:
        setBlock(BLOCK_I, blockI);
        break;
    case 1:
        setBlock(BLOCK_T, blockT);
        break;
    case 2:
        setBlock(BLOCK_L, blockL);
        break;
    case 3:
        setBlock(BLOCK_L2, blockL2);
        break;
    case 4:
        setBlock(BLOCK_O, blockO);
        break;
    case 5:
        setBlock(BLOCK_S, blockS);
        break;
    case 6:
        setBlock(BLOCK_S2, blockS2);
        break;
    }
}

void TetrisGameService::setBlock(int blockType, const int block[][4])
{
    currentBlockType = blockType;
    memcpy(currentBlock, block, 4 * 4 * sizeof(int));
}

void TetrisGameService::updateBoard(int nextBlockTopLeftY, int nextBlockTopLeftX, int nextBlock[][4])
{
    if (isBlockCollide(nextBlockTopLeftY, nextBlockTopLeftX, nextBlock))
    {
        if (isBlockCollideBottom(nextBlockTopLeftY, nextBlock))
        {
            updateGameState();
        }
        return;
    }
    doUpdateBoard(nextBlockTopLeftY, nextBlockTopLeftX, nextBlock);
}

bool TetrisGameService::isBlockCollide(int nextBlockTopLeftY, int nextBlockTopLeftX, int nextBlock[][4])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (nextBlock[i][j] == 1 && backBoard[nextBlockTopLeftY + i][BACKBOARD_BARRIER_SIZE + nextBlockTopLeftX + j] == 1)
            {
                return true;
            }
        }
    }
    return false;
}

bool TetrisGameService::isBlockCollideBottom(int nextBlockTopLeftY, int nextBlock[][4])
{
    if (nextBlockTopLeftY == blockTopLeftY)
    {
        return false;
    }
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (nextBlock[i][j] != 1)
            {
                continue;
            }
            if (backBoard[nextBlockTopLeftY + i][blockTopLeftX + j] == 1)
            {
                return true;
            }
        }
    }
    return false;
}

void TetrisGameService::doUpdateBoard(int nextBlockTopLeftY, int nextBlockTopLeftX, int nextBlock[][4])
{
    TetrisBoard tempBoard = TetrisBoard(backBoard);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (nextBlock[i][j])
            {
                backBoard[nextBlockTopLeftY + i][BACKBOARD_BARRIER_SIZE + nextBlockTopLeftX + j] = nextBlock[i][j];
            }
        }
    }
    // debugPrintBoard(backBoard);
    (*board) = extractBoardFromBackBoard();
    blockTopLeftY = nextBlockTopLeftY;
    blockTopLeftX = nextBlockTopLeftX;
    memcpy(currentBlock, nextBlock, 4 * 4 * sizeof(int));
    backBoard = tempBoard;
}

void TetrisGameService::breakFullLineFromBoard()
{
    for (int i = 0; i < boardHeight; ++i)
    {
        bool isFullLine = true;
        for (int j = 0; j < boardWidth; ++j)
        {
            if ((*board)[i][j] != 1)
            {
                isFullLine = false;
                break;
            }
        }
        if (isFullLine)
        {
            breakLine(i);
        }
    }
}

void TetrisGameService::breakLine(int fullLineIdx)
{
    for (int i = fullLineIdx - 1; i >= 0; --i)
    {
        for (int j = 0; j < boardWidth; ++j)
        {
            (*board)[i + 1][j] = (*board)[i][j];
        }
    }
    for (int i = 0; i < boardWidth; ++i)
    {
        (*board)[0][i] = 0;
    }
}

void TetrisGameService::rotateBlock(int nextBlock[][4])
{
    int sideLength;
    switch (currentBlockType)
    {
    case BLOCK_O:
        sideLength = 2;
        break;
    case BLOCK_L:
    case BLOCK_L2:
    case BLOCK_S:
    case BLOCK_S2:
    case BLOCK_T:
        sideLength = 3;
        break;
    case BLOCK_I:
        sideLength = 4;
        break;
    }
    // int tempBlock[4][4];
    memset(nextBlock, 0, 4 * 4 * sizeof(int));
    for (int i = 0; i < sideLength; ++i)
    {
        int k = sideLength - i - 1;
        for (int j = 0; j < sideLength; ++j)
        {
            nextBlock[j][k] = currentBlock[i][j];
        }
    }
}
