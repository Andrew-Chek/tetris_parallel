#include "Game.h"
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <map>

std::mutex renderMutex;

void runGameLogic(Game& game, bool& running) {
    while (running && !game.getGameOver()) {
        game.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

std::mutex inputMutex;
std::mutex buttonMutex;

void handleGameInput(Game& game, const std::string& command) {
    std::lock_guard<std::mutex> lock(inputMutex);
    game.handleInput(command);
}

void handleMouseClick(Game& game, int x, int y) {
    std::lock_guard<std::mutex> lock(buttonMutex);
    game.handleMouseClick(x, y);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* mainWindow = SDL_CreateWindow("Two Player Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!mainWindow) {
        std::cerr << "Failed to create SDL main window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mainRenderer) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(mainWindow);
        SDL_Quit();
        return 1;
    }

    Game game1;
    Game game2;

    if (!game1.initialize() || !game2.initialize()) {
        std::cerr << "Failed to initialize one or both game instances." << std::endl;
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainWindow);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    bool game1Running = true;
    bool game2Running = true;

    SDL_Event event;

    std::thread game1Thread(runGameLogic, std::ref(game1), std::ref(game1Running));
    std::thread game2Thread(runGameLogic, std::ref(game2), std::ref(game2Running));

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                game1Running = false;
                game2Running = false;
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    Uint32 closedWindowID = event.window.windowID;

                    if (closedWindowID == SDL_GetWindowID(game1.getWindow())) {
                        std::cout << "Game1 window closed." << std::endl;
                        game1Running = false;
                        SDL_DestroyWindow(game1.getWindow());
                    }
                    else if (closedWindowID == SDL_GetWindowID(game2.getWindow())) {
                        std::cout << "Game2 window closed." << std::endl;
                        game2Running = false;
                        SDL_DestroyWindow(game2.getWindow());
                    }
                    else if (closedWindowID == SDL_GetWindowID(mainWindow)) {
                        std::cout << "Main window closed." << std::endl;
                        running = false;
                        game1Running = false;
                        game2Running = false;
                    }
                }
            }

            if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        handleGameInput(game1, "ROTATE");
                        break;
                    case SDLK_a:
                        handleGameInput(game1, "LEFT");
                        break;
                    case SDLK_s:
                        handleGameInput(game1, "DOWN");
                        break;
                    case SDLK_d:
                        handleGameInput(game1, "RIGHT");
                        break;
                    case SDLK_SPACE:
                        handleGameInput(game1, "SPACE");
                        break;

                    case SDLK_UP:
                        handleGameInput(game2, "ROTATE");
                        break;
                    case SDLK_LEFT:
                        handleGameInput(game2, "LEFT");
                        break;
                    case SDLK_DOWN:
                        handleGameInput(game2, "DOWN");
                        break;
                    case SDLK_RIGHT:
                        handleGameInput(game2, "RIGHT");
                        break;
                    case SDLK_RETURN:
                        handleGameInput(game2, "SPACE");
                        break;
                }
            }
        }

        renderMutex.lock();
        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);
        SDL_RenderClear(mainRenderer);

        if (game1Running) game1.render();
        if (game2Running) game2.render();

        SDL_RenderPresent(mainRenderer);
        renderMutex.unlock();

        if (!game1Running && !game2Running) {
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    if (game1Running) game1Thread.join();
    if (game2Running) game2Thread.join();

    game1.cleanup();
    game2.cleanup();

    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}
