#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <SDL2/SDL_ttf.h>
#include "Game.h"
#include "Constants.h"

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      currentTetromino(nullptr),
      quit(false),
      gameOver(false),
      score(0),
      restartButtonClicked(false),
      paused(false),
      lastTick(0) {
    restartButtonRect = {200, 300, 200, 50}; // Example restart button dimensions
}

// Destructor
Game::~Game() {
    delete currentTetromino;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    spawnTetromino();
    return true;
}

void Game::spawnTetromino() {
    // Randomly select a Tetromino type
    TetrominoType type = static_cast<TetrominoType>(rand() % 7); // 7 types: I, O, T, S, Z, J, L

    // Get the shape for that type
    const auto& shape = TETROMINOS[static_cast<int>(type)];

    // Starting position for the Tetromino
    Position startPos(COLS / 2 - static_cast<int>(shape[0].size()) / 2, 0);

    // Create a new Tetromino with the selected type, shape, and start position
    currentTetromino = new Tetromino(type, shape, startPos);

    // Check for collision at the spawn location
    if (board.checkCollision(*currentTetromino, 0, 0)) {
        gameOver = true; // Game Over if the spawn collides
    }
}

void Game::handleInput(const std::string& command) {
    if (gameOver) return;

    if (command == "LEFT") {
        if (!board.checkCollision(*currentTetromino, -1, 0)) {
            currentTetromino->move(-1, 0);
        }
    } else if (command == "RIGHT") {
        if (!board.checkCollision(*currentTetromino, 1, 0)) {
            currentTetromino->move(1, 0);
        }
    } else if (command == "DOWN") {
        if (!board.checkCollision(*currentTetromino, 0, 1)) {
            currentTetromino->move(0, 1);
        }
    } else if (command == "ROTATE") {
        currentTetromino->rotate();
        if (board.checkCollision(*currentTetromino, 0, 0)) {
            currentTetromino->rotate();
            currentTetromino->rotate();
            currentTetromino->rotate(); // Reverse rotation
        }
    } else if (command == "SPACE") {
        // Immediate drop of the Tetromino
        while (!board.checkCollision(*currentTetromino, 0, 1)) {
            currentTetromino->move(0, 1);
        }
        board.mergeTetromino(*currentTetromino);  // Merge the Tetromino once it hits the bottom
        int count = board.clearLines();  // Check if any lines need to be cleared
        calculateScore(count);
        spawnTetromino();  // Spawn a new Tetromino
    }
}

void Game::calculateScore(int count) {
    score += count * 15;
}

std::string Game::serializeBoard() const {
    return board.serializeBoard();
}

SDL_Color Game::getTetrominoColor(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return {0, 255, 255, 255}; // Cyan
        case TetrominoType::O: return {255, 255, 0, 255}; // Yellow
        case TetrominoType::T: return {128, 0, 128, 255}; // Purple
        case TetrominoType::S: return {0, 255, 0, 255};   // Green
        case TetrominoType::Z: return {255, 0, 0, 255};   // Red
        case TetrominoType::J: return {0, 0, 255, 255};   // Blue
        case TetrominoType::L: return {255, 165, 0, 255}; // Orange
    }
    return {255, 255, 255, 255}; // Default white
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    TTF_Font* font = TTF_OpenFont("../assets/Roboto-Thin.ttf", 24);

    // Draw board with cells
    const auto& grid = board.getGrid();
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            SDL_Rect cell = {col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            if (grid[row][col] == 0) {
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark grid color
                SDL_RenderFillRect(renderer, &cell);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White grid lines
                SDL_RenderDrawRect(renderer, &cell);
            } else {
                // Render locked figure with a slightly darker gray
                SDL_SetRenderDrawColor(renderer, LOCKED_COLOR.r, LOCKED_COLOR.g, LOCKED_COLOR.b, 255);
                SDL_RenderFillRect(renderer, &cell);

                // Add a rectangle outline around locked figure cells
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light gray outline
                SDL_RenderDrawRect(renderer, &cell); // Draw rectangle outline
            }
        }
    }

    const auto& shape = currentTetromino->getShape();
    const auto& pos = currentTetromino->getPosition();

    // Draw the current tetromino with appropriate color
    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col]) {
                SDL_Rect cell = {
                    static_cast<int>((pos.x + col) * CELL_SIZE),
                    static_cast<int>((pos.y + row) * CELL_SIZE),
                    CELL_SIZE,
                    CELL_SIZE
                };

                // Use the tetromino color based on its type
                SDL_Color tetrominoColor = getTetrominoColor(currentTetromino->getType());
                SDL_SetRenderDrawColor(renderer, tetrominoColor.r, tetrominoColor.g, tetrominoColor.b, 255);
                SDL_RenderFillRect(renderer, &cell);

                // Optionally, you can draw the border around the tetromino cells
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for border
                SDL_RenderDrawRect(renderer, &cell);
            }
        }
    }

    // Render score section
    int SCORE_AREA_WIDTH = 200;
    int SCORE_AREA_X = WINDOW_WIDTH - SCORE_AREA_WIDTH;
    int SCORE_Y = 50;

    SDL_Rect scoreBackground = {SCORE_AREA_X, 0, SCORE_AREA_WIDTH, WINDOW_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Dark gray for background
    SDL_RenderFillRect(renderer, &scoreBackground);

    renderText("Score: " + std::to_string(score), font, SCORE_AREA_X + (SCORE_AREA_WIDTH / 2) - 50, SCORE_Y);

    // Render "Pause" button below the score
    SDL_Rect pauseButtonRect = {SCORE_AREA_X + (SCORE_AREA_WIDTH - 140) / 2, SCORE_Y + 100, 140, 50};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray for button
    SDL_RenderFillRect(renderer, &pauseButtonRect);
    renderText("Pause", font, pauseButtonRect.x + 25, pauseButtonRect.y + 10);

    SDL_RenderPresent(renderer);

    if (gameOver) {
        renderGameOverScreen();
        return; // Skip the rest of the game rendering logic
    }
}

// Function to check if mouse click is within the restart button area
void Game::handleMouseClick(int x, int y) {
    if (gameOver) {
        // Check if the mouse click is inside the restart button's rectangle
        if (x >= restartButtonRect.x && x <= restartButtonRect.x + restartButtonRect.w &&
            y >= restartButtonRect.y && y <= restartButtonRect.y + restartButtonRect.h) {
            restartGame(); // Restart the game if the button is clicked
        }
    } else if (!paused) {
        std::cout << "Coordinates:" << x << ", " << y << std::endl;
        // Check if the mouse click is inside the pause button's rectangle
        int SCORE_AREA_WIDTH = 200;
        int SCORE_AREA_X = WINDOW_WIDTH - SCORE_AREA_WIDTH;
        SDL_Rect pauseButtonRect = {SCORE_AREA_X + 30, 150, 140, 50};

        std::cout << "Pause button coordinates:" << pauseButtonRect.x << ", " << pauseButtonRect.y << std::endl;

        if (x >= pauseButtonRect.x && x <= pauseButtonRect.x + pauseButtonRect.w &&
            y >= pauseButtonRect.y && y <= pauseButtonRect.y + pauseButtonRect.h) {
            std::cout << "I'm here" << std::endl;
            togglePause(); // Pause the game
        }
    } else {
        // Handle clicks on the "Resume" button in the pause overlay
        SDL_Rect resumeButtonRect = {(WINDOW_WIDTH - 200) / 2, (WINDOW_HEIGHT - 50) / 2, 200, 50};

        if (x >= resumeButtonRect.x && x <= resumeButtonRect.x + resumeButtonRect.w &&
            y >= resumeButtonRect.y && y <= resumeButtonRect.y + resumeButtonRect.h) {
            togglePause(); // Resume the game
        }
    }
}

void Game::renderGameOverScreen() {
    // Clear the entire screen before rendering the overlay
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Fully clear with black color
    SDL_RenderClear(renderer);
    // Render a black semi-transparent overlay covering the entire window
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);  // Semi-transparent black
    SDL_Rect overlayRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &overlayRect);

    // Render "Game Over" text
    TTF_Font* font = TTF_OpenFont("../assets/Roboto-Thin.ttf", 48);
    SDL_Color textColor = {255, 0, 0, 255}; // Red color for "Game Over"
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game Over", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {(WINDOW_WIDTH - textSurface->w) / 2, (WINDOW_HEIGHT - textSurface->h) / 3, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Render the restart button
    renderRestartButton();

    // Update the window
    SDL_RenderPresent(renderer);
}
    // Reset game state for restart
void Game::restartGame() {
    // Reset game state
    gameOver = false;   // Set game over flag to false
    score = 0;          // Reset the score to 0

    // Reset the board (clear it)
    board.clear();  // Assuming you have a method in your board class to clear the grid

    spawnTetromino();

    gameLoop();

    // Re-enable input handling or any necessary components
    // isPaused = false;  // If you use a pause feature, you can reset it
}

void Game::renderRestartButton() {
    // Set the button rectangle position and size
    restartButtonRect = { (WINDOW_WIDTH - 200) / 2, WINDOW_HEIGHT / 2 + 50, 200, 50 };

    // Render button background (gray)
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &restartButtonRect);

    // Render button text (White)
    TTF_Font* font = TTF_OpenFont("../assets/Roboto-Thin.ttf", 24);
    SDL_Color textColor = {255, 255, 255, 255}; // White text for the button
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Restart Game", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {(WINDOW_WIDTH - textSurface->w) / 2, WINDOW_HEIGHT / 2 + 60, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game::renderPauseOverlay() {
    std::cout << "Im in render pause overlay" << std::endl;
    // Semi-transparent overlay
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); 
    SDL_Rect overlayRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &overlayRect);

    // Render "Paused" text
    TTF_Font* font = TTF_OpenFont("../assets/Roboto-Thin.ttf", 48);
    SDL_Color textColor = {255, 255, 255, 255}; // White text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game is paused", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {(WINDOW_WIDTH - textSurface->w) / 2, (WINDOW_HEIGHT - textSurface->h) / 3, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    // Render "Resume" button
    SDL_Color buttonColor = {100, 100, 255, 255}; // Blue button
    SDL_Rect resumeButtonRect = {(WINDOW_WIDTH - 200) / 2, (WINDOW_HEIGHT - 50) / 2, 200, 50};
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &resumeButtonRect);

    // Render "Resume" button text
    SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Resume", textColor);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);

    SDL_Rect buttonTextRect = {resumeButtonRect.x + (resumeButtonRect.w - buttonSurface->w) / 2, 
                               resumeButtonRect.y + (resumeButtonRect.h - buttonSurface->h) / 2, 
                               buttonSurface->w, buttonSurface->h};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &buttonTextRect);

    // Clean up
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(buttonSurface);
    SDL_DestroyTexture(buttonTexture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
}

void Game::togglePause() {
    std::cout << "Im here as well!!!!!!!!!!!!!!!" << std::endl;
    std::cout << paused << std::endl;
    paused = !paused;
    std::cout << paused << std::endl;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);
    if (paused) {
        // Display "Paused" overlay
        renderPauseOverlay();
    } else {
        // Resume game, continue rendering and updating
        render();
    }

    SDL_RenderPresent(renderer); 
}
// Check for game over condition during tetromino falling logic
void Game::checkGameOver() {
    if (board.checkCollision(*currentTetromino, 0, currentTetromino->getPosition().y)) {
        gameOver = true; // Set game over if collision occurs
    }
}

bool Game::isGameActive() {
    return !gameOver;
}

void Game::setGameOver(bool flag) {
    gameOver = flag;
}

void Game::renderText(const std::string& text, TTF_Font* font, int x, int y) {
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Render the text to a surface
    SDL_Color white = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    if (!surface) {
        std::cerr << "Failed to create text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Convert the surface to a texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create text texture! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    // Determine the text rectangle
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    // Cleanup
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::update(){
    if(gameOver) return;

    Uint32 currentTick = SDL_GetTicks();
    
    // Move the tetromino downward every tickInterval milliseconds
    if (currentTick - lastTick >= tickInterval) {
        // Check if the tetromino can move down
        if (!board.checkCollision(*currentTetromino, 0, 1)) {
            currentTetromino->move(0, 1); // Move the tetromino down
        } else {
            // Merge tetromino into board and spawn a new one
            board.mergeTetromino(*currentTetromino);
            int count = board.clearLines();
            calculateScore(count);
            spawnTetromino();
        }

        lastTick = currentTick; // Update lastTick to the current time
    }

}

void Game::run(const std::vector<std::string>& commands) {
    lastTick = SDL_GetTicks();
    for(const auto& command : commands){
        if (gameOver || quit){
            break;
        }
        handleInput(command);
        update();
        render();
        SDL_Delay(16); // Cap at ~60 FPS
    }
}

void Game::updateBoard(const std::string& serializedBoard) {
    board.deserializeBoard(serializedBoard);
}

void Game::cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void Game::gameLoop() {
    while (!quit) {
        Uint32 currentTick = SDL_GetTicks();

        // Game logic runs only when not paused
        if (!paused && (currentTick - lastTick >= tickInterval)) {
            if (!gameOver) {
                currentTetromino->move(0, 1);
                if (board.checkCollision(*currentTetromino, 0, 1)) {
                    board.mergeTetromino(*currentTetromino);
                    int count = board.clearLines();
                    calculateScore(count);
                    spawnTetromino();
                }
            }
            render();
            lastTick = currentTick;
        }

        // Handle input (key press)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } 
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                handleMouseClick(event.button.x, event.button.y);
            } 
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT) {
                    handleInput("LEFT");
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    handleInput("RIGHT");
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    handleInput("DOWN");
                } else if (event.key.keysym.sym == SDLK_UP) {
                    handleInput("ROTATE");
                } else if (event.key.keysym.sym == SDLK_SPACE) {
                    handleInput("SPACE");
                }
            }
        }
    }
}