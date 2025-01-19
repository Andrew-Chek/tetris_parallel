#include <unistd.h>
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include "../include/lib/windowService.hpp"
#include "../include/lib/keyboardService.hpp"
#include "../include/lib/tetrisGameService.hpp"

#define TETRIS_BOARD_TOP_LEFT_Y 2
#define TETRIS_BOARD_TOP_LEFT_X 10
#define TETRIS_BOARD_WIDTH 16
#define TETRIS_BOARD_HEIGHT 16
#define FPS 60


#define BLOCK_FILLED 1

int main()
{
const double frameTime = 1 / (double)FPS * 1e3; // ms

    TetrisBoard* tetrisBoard = new TetrisBoard(TETRIS_BOARD_HEIGHT, std::vector<int>(TETRIS_BOARD_WIDTH, 0));

    KeyboardService keyboardService = KeyboardService();
    WindowService windowService = WindowService();
    TetrisGameService tetrisGameService = TetrisGameService(tetrisBoard);

    windowService.registerStyle(BLOCK_FILLED, "█");

    std::thread t1(&KeyboardService::run, keyboardService);

    for (;;)
    {
        std::queue<char> keys = keyboardService.getKeys();
        tetrisGameService.accept(&keys);

        windowService.addComponent(tetrisBoard, TETRIS_BOARD_TOP_LEFT_Y, TETRIS_BOARD_TOP_LEFT_X);

        if (tetrisGameService.getGameState() == TetrisGameState::Gameover) {
            windowService.addComponent("Game Over", TETRIS_BOARD_TOP_LEFT_Y + TETRIS_BOARD_HEIGHT + 2, TETRIS_BOARD_TOP_LEFT_X);
            windowService.addComponent("Press any key to exit..", TETRIS_BOARD_TOP_LEFT_Y + TETRIS_BOARD_HEIGHT + 3, TETRIS_BOARD_TOP_LEFT_X);
            windowService.render();
            break;
        }

        windowService.render();
        usleep(frameTime * 1e3); // us
    }



    keyboardService.dispose();

    // std::cout << "Press any key to exit..\n";
    t1.join();

    return 0;

}