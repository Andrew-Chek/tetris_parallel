#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <netinet/in.h>
#include <vector>
#include <cmath>
#include <cerrno>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

class Tetris2PServer
{
public:
    void run()
    {
        openListenSocket();
        waitForTwoClients();
        startRelayLoop();
    }

private:
    std::vector<int> clientSockets;
    int masterSocket;
    struct sockaddr_in address;
    void openListenSocket()
    {
        printf("open listen socket\n");
        if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("masterCocket failed");
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                       sizeof(opt)) < 0)
        {
            perror("setsockopt failed");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);
        if (bind(masterSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(masterSocket, 2) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        printf("succeed open listen socket\n");
    }
    void waitForTwoClients()
    {
        printf("wait for two clilents\n");
        while (clientSockets.size() < 2)
        {
            int newSocket;
            int addrlen;
            if ((newSocket = accept(masterSocket, (struct sockaddr *)&address,
                                    (socklen_t *)&addrlen)) < 0)
            {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n", newSocket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            clientSockets.push_back(newSocket);
        }
        printf("accepted two clilents\n");
    }
    void startRelayLoop()
    {
        printf("start relay loop\n");
        while (true)
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            for (int clientSocket : clientSockets)
            {
                FD_SET(clientSocket, &readfds);
            }
            if (select(clientSockets.back() + 1, &readfds, NULL, NULL, NULL) < 0 && errno != EINTR)
            {
                printf("select failed");
            }
            // relayData;
            for (int i = 0; i < clientSockets.size(); ++i)
            {
                int clientSocket = clientSockets[i];
                if (!FD_ISSET(clientSocket, &readfds))
                {
                    continue;
                }
                int addrlen;
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                if ((read(clientSocket, buffer, 1024)) == 0)
                {
                    // Somebody disconnected , get his details and print
                    getpeername(clientSocket, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    // Close the socket and mark as 0 in list for reuse
                    close(clientSocket);
                    perror("The number of players must be 2");
                    exit(EXIT_FAILURE);
                }
                int anotherSocket = clientSockets[clientSockets.size() - i - 1];
                send(anotherSocket, buffer, strlen(buffer), 0);
            }
        }
    }
};

int main()
{
    Tetris2PServer server;
    server.run();
}