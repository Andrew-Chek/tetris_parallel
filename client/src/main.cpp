#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <netinet/in.h>
#include <unistd.h>

constexpr int PORT = 5000;

bool connectToServer(int& serverSocket) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket.\n";
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect to server.\n";
        close(serverSocket);
        return false;
    }

    return true;
}

void clientGameLoop(int serverSocket) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Tetris Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 800, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    char buffer[1024];
    while (true) {
        // Receive game state from server
        int bytesReceived = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Disconnected from server.\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        std::string gameState(buffer);

        // Render the received game state (you can enhance this logic)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Parse and display the board (example visualization)
        int y = 0;
        for (char c : gameState) {
            if (c == '\n') {
                ++y;
                continue;
            }

            int x = (&c - &gameState[0]) % 10; // Example for grid width 10
            if (c == '1') {
                SDL_Rect block = {x * 40, y * 40, 40, 40};
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &block);
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(33); // Simulate ~30 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    int serverSocket;
    if (!connectToServer(serverSocket)) {
        return -1;
    }

    clientGameLoop(serverSocket);

    close(serverSocket);
    return 0;
}
