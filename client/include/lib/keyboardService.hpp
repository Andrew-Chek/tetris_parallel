#ifndef KEYBOARD_SERVICE_HPP
#define KEYBOARD_SERVICE_HPP

#include <unistd.h>
#include <termios.h>
#include <cstdio>
#include <vector>
#include <queue>
#include <mutex>
#include "./common/sharedQueue.hpp"

class KeyboardService
{
public:
    KeyboardService();
    void dispose();
    void *run();
    std::queue<char> getKeys();

private:
    bool* disposed;
    SharedQueue<char> *q;
    char getInput();
};

#endif
