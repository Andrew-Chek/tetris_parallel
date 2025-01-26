// #include <iostream>
// #include <thread>
// #include <vector>
// #include <memory>
// #include <mutex>
// #include <atomic>
// #include "Game.h"
// // Handles a single game session
// void handleGameSession(int clientSocket) {
//     Game game;
//     if (!game.initialize()) {
//         std::cerr << "Game initialization failed for client" << std::endl;
//         return;
//     }

//     game.gameLoop();  // Run the game loop for the current client
// }

// // Main function for the server
// int main() {
//     GameServer server;
//     server.start(12345);  // Listen on port 12345 for incoming connections

//     // Wait for clients to connect and start a game for each pair of players
//     while (true) {
//         int clientSocket = server.acceptClient();  // Accept a new client
//         std::thread gameThread(handleGameSession, clientSocket);
//         gameThread.detach();  // Start each game in its own thread
//     }

//     return 0;
// }
