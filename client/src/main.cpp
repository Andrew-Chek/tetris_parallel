#include "Game.h"
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <map>

class GameManager {
public:
    GameManager()
        : running(true), game1Running(true), game2Running(true) {}

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
            cleanup();
            return false;
        }

        return true;
    }

    void run() {
        std::thread game1Thread(&GameManager::runGameLogic, this, std::ref(game1), std::ref(game1Running));
        std::thread game2Thread(&GameManager::runGameLogic, this, std::ref(game2), std::ref(game2Running));

        while (running) {
            processEvents();
            render();

            if (!game1Running && !game2Running) {
                running = false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        if (game1Running) game1Thread.join();
        if (game2Running) game2Thread.join();

        cleanup();
    }

private:
    SDL_Window* mainWindow = nullptr;
    SDL_Renderer* mainRenderer = nullptr;

    Game game1;
    Game game2;

    bool running;
    bool game1Running;
    bool game2Running;

    std::mutex renderMutex;
    std::mutex inputMutex;
    std::mutex buttonMutex;

    void runGameLogic(Game& game, bool& gameRunning) {
        while (gameRunning && !game.getGameOver()) {
            game.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void processEvents() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                game1Running = false;
                game2Running = false;
            } else if (event.type == SDL_WINDOWEVENT) {
                handleWindowEvent(event);
            } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                handleKeyboardInput(event);
            }
        }
    }

    void handleWindowEvent(const SDL_Event& event) {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            Uint32 closedWindowID = event.window.windowID;

            if (closedWindowID == SDL_GetWindowID(game1.getWindow())) {
                std::cout << "Game1 window closed." << std::endl;
                game1Running = false;
                SDL_DestroyWindow(game1.getWindow());
            } else if (closedWindowID == SDL_GetWindowID(game2.getWindow())) {
                std::cout << "Game2 window closed." << std::endl;
                game2Running = false;
                SDL_DestroyWindow(game2.getWindow());
            } else if (closedWindowID == SDL_GetWindowID(mainWindow)) {
                std::cout << "Main window closed." << std::endl;
                running = false;
                game1Running = false;
                game2Running = false;
            }
        }
    }

    void handleKeyboardInput(const SDL_Event& event) {
        switch (event.key.keysym.sym) {
            case SDLK_w:
                handleInput(game1, "ROTATE");
                break;
            case SDLK_a:
                handleInput(game1, "LEFT");
                break;
            case SDLK_s:
                handleInput(game1, "DOWN");
                break;
            case SDLK_d:
                handleInput(game1, "RIGHT");
                break;
            case SDLK_SPACE:
                handleInput(game1, "SPACE");
                break;
            case SDLK_p:
                handleInput(game1, "Pause");
                break;

            case SDLK_UP:
                handleInput(game2, "ROTATE");
                break;
            case SDLK_LEFT:
                handleInput(game2, "LEFT");
                break;
            case SDLK_DOWN:
                handleInput(game2, "DOWN");
                break;
            case SDLK_RIGHT:
                handleInput(game2, "RIGHT");
                break;
            case SDLK_RETURN:
                handleInput(game2, "SPACE");
                break;
            case SDLK_q:
                handleInput(game2, "Pause");
                break;
        }
    }

    void handleInput(Game& game, const std::string& command) {
        std::lock_guard<std::mutex> lock(inputMutex);
        game.handleInput(command);
    }

    void render() {
        std::lock_guard<std::mutex> lock(renderMutex);

        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);
        SDL_RenderClear(mainRenderer);

        if (game1Running) game1.render();
        if (game2Running) game2.render();

        SDL_RenderPresent(mainRenderer);
    }

    void cleanup() {
        game1.cleanup();
        game2.cleanup();

        if (mainRenderer) SDL_DestroyRenderer(mainRenderer);
        if (mainWindow) SDL_DestroyWindow(mainWindow);

        SDL_Quit();
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
