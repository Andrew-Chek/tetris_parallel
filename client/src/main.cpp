#include "Game.h"
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <map>

class GameManager {
private:
    Game game1;
    Game game2;

    SDL_Window* mainWindow;
    SDL_Renderer* mainRenderer;

    std::mutex renderMutex;
    std::mutex inputMutex;
    
    bool running;
    bool game1Running;
    bool game2Running;
    bool game1Over;
    bool game2Over;

    std::thread game1Thread;
    std::thread game2Thread;

    void runGameLogic(Game& game, bool& gameRunning, bool& gameOver) {
        while (gameRunning) {
            if (game.getGameOver()) {
                gameOver = true;
                break;
            }
            game.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void handleGameInput(Game& game, const std::string& command) {
        std::lock_guard<std::mutex> lock(inputMutex);
        game.handleInput(command);
    }

    void processEvent(const SDL_Event& event) {
        if (event.type == SDL_QUIT) {
            stopGames();
        } else if (event.type == SDL_WINDOWEVENT) {
            handleWindowEvent(event);
        } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            handleKeyDown(event.key.keysym.sym);
        }
    }

    void handleWindowEvent(const SDL_Event& event) {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            Uint32 closedWindowID = event.window.windowID;

            if (closedWindowID == SDL_GetWindowID(game1.getWindow())) {
                game1Running = false;
                SDL_DestroyWindow(game1.getWindow());
            } else if (closedWindowID == SDL_GetWindowID(game2.getWindow())) {
                game2Running = false;
                SDL_DestroyWindow(game2.getWindow());
            } else if (closedWindowID == SDL_GetWindowID(mainWindow)) {
                stopGames();
            }
        }
    }

    void handleKeyDown(SDL_Keycode key) {
        switch (key) {
            case SDLK_w: handleGameInput(game1, "ROTATE"); break;
            case SDLK_a: handleGameInput(game1, "LEFT"); break;
            case SDLK_s: handleGameInput(game1, "DOWN"); break;
            case SDLK_d: handleGameInput(game1, "RIGHT"); break;
            case SDLK_SPACE: handleGameInput(game1, "SPACE"); break;
            case SDLK_e: handleGameInput(game1, "Pause"); break;

            case SDLK_UP: handleGameInput(game2, "ROTATE"); break;
            case SDLK_LEFT: handleGameInput(game2, "LEFT"); break;
            case SDLK_DOWN: handleGameInput(game2, "DOWN"); break;
            case SDLK_RIGHT: handleGameInput(game2, "RIGHT"); break;
            case SDLK_RETURN: handleGameInput(game2, "SPACE"); break;
            case SDLK_p: handleGameInput(game2, "Pause"); break;

            case SDLK_r:
                restartGames();
                break;

            case SDLK_q:
                stopGames();
                break;
        }
    }

    void renderGames() {
        renderMutex.lock();
        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);
        SDL_RenderClear(mainRenderer);

        if (game1Running) game1.render();
        if (game2Running) game2.render();

        SDL_RenderPresent(mainRenderer);
        renderMutex.unlock();
    }

    void stopGames() {
        running = false;
        game1Running = false;
        game2Running = false;
    }

    void restartGames() {
        if (game1Over && game2Over) {
            game1.cleanup();
            game2.cleanup();

            game1.initialize();
            game2.initialize();

            game1Over = false;
            game2Over = false;

            game1Thread.join();
            game2Thread.join();

            game1Running = true;
            game2Running = true;
            game1Thread = std::thread(&GameManager::runGameLogic, this, std::ref(game1), std::ref(game1Running), std::ref(game1Over));
            game2Thread = std::thread(&GameManager::runGameLogic, this, std::ref(game2), std::ref(game2Running), std::ref(game2Over));
        }
    }

public:
    GameManager() : mainWindow(nullptr), mainRenderer(nullptr), running(true), game1Running(true), game2Running(true), game1Over(false), game2Over(false) {}

    ~GameManager() {
        if (game1Running) game1Thread.join();
        if (game2Running) game2Thread.join();

        game1.cleanup();
        game2.cleanup();

        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainWindow);
        SDL_Quit();
    }

    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        mainWindow = SDL_CreateWindow("Two Player Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
        if (!mainWindow) {
            std::cerr << "Failed to create SDL main window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
        if (!mainRenderer) {
            std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(mainWindow);
            SDL_Quit();
            return false;
        }

        if (!game1.initialize() || !game2.initialize()) {
            std::cerr << "Failed to initialize one or both game instances." << std::endl;
            SDL_DestroyRenderer(mainRenderer);
            SDL_DestroyWindow(mainWindow);
            SDL_Quit();
            return false;
        }

        game1Thread = std::thread(&GameManager::runGameLogic, this, std::ref(game1), std::ref(game1Running), std::ref(game1Over));
        game2Thread = std::thread(&GameManager::runGameLogic, this, std::ref(game2), std::ref(game2Running), std::ref(game2Over));

        return true;
    }

    void run() {
        SDL_Event event;

        while (running) {
            while (SDL_PollEvent(&event)) {
                processEvent(event);
            }

            renderGames();

            if (!game1Running && !game2Running) {
                running = false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
};

int main() {
    GameManager manager;

    if (!manager.initialize()) {
        return 1;
    }

    manager.run();

    return 0;
}
