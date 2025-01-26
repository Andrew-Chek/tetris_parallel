#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Board.h"
#include "Tetromino.h"
#include "Position.h"

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
    int score;
    bool restartButtonClicked;
    SDL_Rect restartButtonRect;
    bool paused;

public:
    Game();
    ~Game();
    SDL_Window* getWindow() { return window; }
    bool getGameOver() { return gameOver; }

    bool initialize();
    void spawnTetromino();
    void handleInput(const std::string& command);
    std::string serializeBoard() const;
    SDL_Color getTetrominoColor(TetrominoType type);
    void calculateScore(int count);
    void render();
    void renderPauseOverlay();
    void handleMouseClick(int x, int y);
    void renderGameOverScreen();
    void restartGame();
    void renderRestartButton();
    void checkGameOver();
    void togglePause();
    void renderText(const std::string& text, TTF_Font* font, int x, int y);
    void update();
    void run(const std::vector<std::string>& commands);
    void updateBoard(const std::string& serializedBoard);
    void cleanup();
    void gameLoop();
    void setGameOver(bool flag);
    bool isGameActive();
};

#endif // GAME_H
