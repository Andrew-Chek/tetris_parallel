#include "Game.h"
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <map>

// Mutex for rendering
std::mutex renderMutex;

// Wrapper to handle each game instance logic
void runGameLogic(Game& game, bool& running) {
    while (running && !game.getGameOver()) {
        game.update();  // Update game logic
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }
}

std::mutex inputMutex;
std::mutex buttonMutex;

// Wrap handleInput calls with a mutex
void handleGameInput(Game& game, const std::string& command) {
    std::lock_guard<std::mutex> lock(inputMutex);
    game.handleInput(command);
}

void handleMouseClick(Game& game, int x, int y) {
    buttonMutex.lock();
    std::cout << "Coordinates:" << x + ", " << y << std::endl;
    game.handleMouseClick(x, y);
    buttonMutex.unlock();
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL window and renderer
    SDL_Window* window = SDL_CreateWindow("Two Player Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize two games
    Game game1;
    Game game2;

    if (!game1.initialize() || !game2.initialize()) {
        std::cerr << "Failed to initialize one or both game instances." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    // Start game logic in separate threads
    std::thread game1Thread(runGameLogic, std::ref(game1), std::ref(running));
    std::thread game2Thread(runGameLogic, std::ref(game2), std::ref(running));

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Route events to the appropriate game instance
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

            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Determine which window received the event
                Uint32 clickedWindowID = SDL_GetWindowID(SDL_GetWindowFromID(event.button.windowID));

                // Process the click for Game 1
                if (clickedWindowID == SDL_GetWindowID(game1.getWindow())) {
                    std::cout << "Game1 handle click" << std::endl;
                    handleMouseClick(game1, event.button.x, event.button.y);
                }
                // Process the click for Game 2
                else if (clickedWindowID == SDL_GetWindowID(game2.getWindow())) {
                    std::cout << "Game2 handle click" << std::endl;
                    handleMouseClick(game2, event.button.x, event.button.y);
                }
            }
        }

        // Render games
        renderMutex.lock();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Clear screen
        SDL_RenderClear(renderer);

        game1.render();
        game2.render();

        SDL_RenderPresent(renderer);
        renderMutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }

    // Wait for threads to finish
    game1Thread.join();
    game2Thread.join();

    // Clean up
    game1.cleanup();
    game2.cleanup();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
