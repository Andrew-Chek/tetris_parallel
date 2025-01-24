#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "../client/src/sdltry.cpp"

constexpr int PORT = 5000;
std::mutex gameMutex;

void gameLoop(int clientSocket) {
    Game game;
    if (!game.initialize()) {
        std::cerr << "Failed to initialize game for client.\n";
        close(clientSocket);
        return;
    }

    while (true) {
        {
            std::lock_guard<std::mutex> lock(gameMutex);
            game.update(); // Server updates the game logic
        }

        // Serialize and send the game state to the client
        std::string gameState = game.serializeBoard();
        if (send(clientSocket, gameState.c_str(), gameState.size(), 0) <= 0) {
            std::cerr << "Failed to send game state to client. Disconnecting.\n";
            break;
        }

        // Simulate game tick rate (e.g., 30 FPS)
        SDL_Delay(33);
    }

    close(clientSocket);
    std::cout << "Client disconnected.\n";
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create server socket.\n";
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Binding failed.\n";
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on socket.\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client.\n";
            continue;
        }

        std::cout << "Client connected.\n";
        std::thread(gameLoop, clientSocket).detach(); // Run game loop for this client
    }

    close(serverSocket);
    return 0;
}
