#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <SDL2/SDL_ttf.h>  

enum class TetrominoType {
    I, O, T, S, Z, J, L
};

// Constants
const int ROWS = 20;
const int COLS = 10;
const int CELL_SIZE = 30;
const int WINDOW_WIDTH = COLS * CELL_SIZE + 200;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;
const SDL_Color I_COLOR = {0, 255, 255}; // Cyan
const SDL_Color O_COLOR = {255, 255, 0}; // Yellow
const SDL_Color T_COLOR = {128, 0, 128}; // Purple
const SDL_Color S_COLOR = {0, 255, 0}; // Green
const SDL_Color Z_COLOR = {255, 0, 0}; // Red
const SDL_Color J_COLOR = {0, 0, 255}; // Blue
const SDL_Color L_COLOR = {255, 165, 0}; // Orange

// Define a color for locked figures (light gray)
const SDL_Color LOCKED_COLOR = {169, 169, 169}; // Light gray

const std::vector<std::vector<std::vector<int>>> TETROMINOS = {
    {  // I shape
        {{1, 1, 1, 1}}
    },
    {  // O shape
        {{1, 1},
         {1, 1}}
    },
    {  // T shape
        {{0, 1, 0},
         {1, 1, 1}}
    },
    {  // S shape
        {{0, 1, 1},
         {1, 1, 0}}
    },
    {  // Z shape
        {{1, 1, 0},
         {0, 1, 1}}
    },
    {  // J shape
        {{1, 0, 0},
         {1, 1, 1}}
    },
    {  // L shape
        {{0, 0, 1},
         {1, 1, 1}}
    }
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
    TetrominoType type;

public:
    Tetromino(TetrominoType type, const std::vector<std::vector<int>>& shape, const Position& pos) 
        : type(type), shape(shape), position(pos) {}

    const std::vector<std::vector<int>>& getShape() const {
        return shape;
    }

    const Position& getPosition() const {
        return position;
    }

    TetrominoType getType() const {
        return type;
    }

    void setPosition(const Position& pos) {
        position = pos;
    }

    void move(int dx, int dy) {
        position.x += dx;
        position.y += dy;
    }

    void rotate() {
        std::vector<std::vector<int>> rotated(shape[0].size(), std::vector<int>(shape.size()));
        for (size_t row = 0; row < shape.size(); ++row) {
            for (size_t col = 0; col < shape[row].size(); ++col) {
                rotated[col][shape.size() - 1 - row] = shape[row][col];
            }
        }
        shape = rotated;
    }
};

class Board {
private:
    std::vector<std::vector<int>> grid;

public:
    Board() : grid(ROWS, std::vector<int>(COLS, 0)) {}

    const std::vector<std::vector<int>>& getGrid() const {
        return grid;
    }

    std::string serializeBoard() const {
        std::string state;
        for (const auto& row : grid) {
            for (int cell : row) {
                state += std::to_string(cell) + " ";
            }
            state += "\n";
        }
        return state;
    }

    void deserializeBoard(const std::string& serializedBoard) {
        std::istringstream ss(serializedBoard);
        std::string line;
        int row = 0;

        // Parse the serialized board string line by line
        while (std::getline(ss, line) && row < ROWS) {
            std::istringstream rowStream(line);
            int col = 0;
            int value;
            while (rowStream >> value && col < COLS) {
                grid[row][col] = value;
                ++col;
            }
            ++row;
        }
    }

    int getDropDistance(const Tetromino& tetromino) {
    const auto& shape = tetromino.getShape();
    const auto& pos = tetromino.getPosition();
    int dropDistance = 0;

    // Simulate dropping the tetromino one row at a time
    while (true) {
        bool collision = false;

        for (size_t row = 0; row < shape.size(); ++row) {
            for (size_t col = 0; col < shape[row].size(); ++col) {
                if (shape[row][col]) {
                    int newX = pos.x + col;
                    int newY = pos.y + dropDistance + row;

                    // Check if the new position is out of bounds or collides with the board
                    if (newY >= ROWS || newX < 0 || newX >= COLS || grid[newY][newX] != 0) {
                        collision = true;
                        break;
                    }
                }
            }
            if (collision) break;
        }

        if (collision) break;

        dropDistance++;
    }

    return dropDistance - 1; // Subtract 1 because the last move caused a collision
}

    bool checkCollision(const Tetromino& tetromino, int offsetX, int offsetY) const {
        const auto& shape = tetromino.getShape();
        const auto& pos = tetromino.getPosition();

        for (size_t row = 0; row < shape.size(); ++row) {
            for (size_t col = 0; col < shape[row].size(); ++col) {
                if (shape[row][col]) {
                    int newX = pos.x + col + offsetX;
                    int newY = pos.y + row + offsetY;

                    if (newX < 0 || newX >= COLS || newY >= ROWS || (newY >= 0 && grid[newY][newX])) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void mergeTetromino(const Tetromino& tetromino) {
        const auto& shape = tetromino.getShape();
        const auto& pos = tetromino.getPosition();

        for (size_t row = 0; row < shape.size(); ++row) {
            for (size_t col = 0; col < shape[row].size(); ++col) {
                if (shape[row][col]) {
                    grid[pos.y + row][pos.x + col] = shape[row][col];
                }
            }
        }
    }

    void clearLines() {
        for (int row = ROWS - 1; row >= 0; --row) {
            bool fullLine = true;
            for (int col = 0; col < COLS; ++col) {
                if (!grid[row][col]) {
                    fullLine = false;
                    break;
                }
            }

            if (fullLine) {
                for (int moveRow = row; moveRow > 0; --moveRow) {
                    grid[moveRow] = grid[moveRow - 1];
                }
                grid[0] = std::vector<int>(COLS, 0);
                ++row; // Re-check this row
            }
        }
    }

    void clear() {
        // Iterate through each cell in the grid and set it to 0 (or another value that represents an empty cell)
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                grid[row][col] = 0;  // Assuming 0 represents an empty cell on the grid
            }
        }
    }
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
    int score;
    bool isCalculatedDropY = false;
    int calculatedDropY = 0;
    bool restartButtonClicked = false; // To track if restart button is clicked
    SDL_Rect restartButtonRect = {100, 300, 200, 50}; // Button size and position (example)

public:
    Game() : window(nullptr), renderer(nullptr), quit(false), gameOver(false), lastTick(0), score(0) {
        srand(static_cast<unsigned>(time(0)));
    }

    ~Game() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
        delete currentTetromino;
    }

    bool initialize() {
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

    void spawnTetromino() {
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

    void handleInput(const std::string& command) {
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
            board.clearLines();  // Check if any lines need to be cleared
            spawnTetromino();  // Spawn a new Tetromino
        }
    }

    std::string serializeBoard() const {
        return board.serializeBoard();
    }

    SDL_Color getTetrominoColor(TetrominoType type) {
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

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        TTF_Font* font = TTF_OpenFont("assets/Roboto-Thin.ttf", 24);

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

        // // Draw shadow for the current tetromino
        const auto& shape = currentTetromino->getShape();
        const auto& pos = currentTetromino->getPosition();

        // // Inside your game loop or function:
        // if (!isCalculatedDropY) {
        //     // Perform the calculation of dropY only once, when the tetromino first appears
        //     int tempDropY = pos.y;
        //     while (!board.checkCollision(*currentTetromino, 0, tempDropY + 1)) {
        //         tempDropY++;  // Increment Y position until it hits a collision
        //     }
        //     calculatedDropY = tempDropY;  // Store the final drop position
        //     isCalculatedDropY = true;  // Set the flag to true to avoid recalculating
        // }
        // // Now draw the shadow at the final drop position (the bottom-most available position)
        // for (size_t row = 0; row < shape.size(); ++row) {
        //     for (size_t col = 0; col < shape[row].size(); ++col) {
        //         if (shape[row][col]) {
        //             // Draw the shadow at the position where the piece will land
        //             SDL_Rect cell = {
        //                 static_cast<int>((pos.x + col) * CELL_SIZE), // X position remains the same
        //                 static_cast<int>((calculatedDropY + row) * CELL_SIZE), // Use dropY for Y position (final resting place)
        //                 CELL_SIZE,
        //                 CELL_SIZE
        //             };
        //             SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100); // White shadow with reduced opacity
        //             SDL_RenderFillRect(renderer, &cell);
        //         }
        //     }
        // }

        // if (board.checkCollision(*currentTetromino, 0, pos.y)) {
        //     // If it collides with the board, it is about to lock
        //     // Update the flag to stop updating dropY during rendering
        //     isCalculatedDropY = false;
        // }

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

        // Display the score on the right
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int SCORE_X = COLS * CELL_SIZE + 20;
        int SCORE_Y = 50;
        SDL_Rect scoreRect = {SCORE_X, SCORE_Y, 140, 50};
        SDL_RenderFillRect(renderer, &scoreRect);
        renderText(std::to_string(score), font, SCORE_X + 10, SCORE_Y + 10);

        SDL_RenderPresent(renderer);

        if (gameOver) {
            renderGameOverScreen();
            return; // Skip the rest of the game rendering logic
        }
    }

    // Function to check if mouse click is within the restart button area
    void handleMouseClick(int x, int y) {
        if (gameOver) {
            // Check if the mouse click is inside the restart button's rectangle
            if (x >= restartButtonRect.x && x <= restartButtonRect.x + restartButtonRect.w &&
                y >= restartButtonRect.y && y <= restartButtonRect.y + restartButtonRect.h) {
                restartGame(); // Restart the game if the button is clicked
            }
        }
    }

    void renderGameOverScreen() {
        // Clear the entire screen before rendering the overlay
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Fully clear with black color
        SDL_RenderClear(renderer);
        // Render a black semi-transparent overlay covering the entire window
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);  // Semi-transparent black
        SDL_Rect overlayRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &overlayRect);

        // Render "Game Over" text
        TTF_Font* font = TTF_OpenFont("assets/Roboto-Thin.ttf", 48);
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
    void restartGame() {
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

    void renderRestartButton() {
        // Set the button rectangle position and size
        restartButtonRect = { (WINDOW_WIDTH - 200) / 2, WINDOW_HEIGHT / 2 + 50, 200, 50 };

        // Render button background (gray)
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &restartButtonRect);

        // Render button text (White)
        TTF_Font* font = TTF_OpenFont("assets/Roboto-Thin.ttf", 24);
        SDL_Color textColor = {255, 255, 255, 255}; // White text for the button
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Restart Game", textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {(WINDOW_WIDTH - textSurface->w) / 2, WINDOW_HEIGHT / 2 + 60, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    // Check for game over condition during tetromino falling logic
    void checkGameOver() {
        if (board.checkCollision(*currentTetromino, 0, currentTetromino->getPosition().y)) {
            gameOver = true; // Set game over if collision occurs
        }
    }


    void renderText(const std::string& text, TTF_Font* font, int x, int y) {
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

    void update(){
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
                board.clearLines();
                spawnTetromino();
            }

            lastTick = currentTick; // Update lastTick to the current time
        }

    }

    void run(const std::vector<std::string>& commands) {
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

    void updateBoard(const std::string& serializedBoard) {
        board.deserializeBoard(serializedBoard);
    }

    void cleanup() {
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

    void gameLoop() {
        while (!quit) {
            Uint32 currentTick = SDL_GetTicks();
            if (currentTick - lastTick >= tickInterval) {
                if (!gameOver) {
                    currentTetromino->move(0, 1);
                    if (board.checkCollision(*currentTetromino, 0, 1)) {
                        board.mergeTetromino(*currentTetromino);
                        board.clearLines();
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
};

int main() {
    Game game;
    if (!game.initialize()) {
        return 1;
    }
    game.gameLoop();
    return 0;
}

