
#include "../../include/lib/keyboardService.hpp"

#include <iostream>

KeyboardService::KeyboardService()
{
    q = new SharedQueue<char>();
    disposed = new bool;
    (*disposed) = false;
}
void *KeyboardService::run()
{
    while (!(*disposed))
    {
        char ch = getInput();
        q->enqueue(ch);
    }
    return nullptr;
}
std::queue<char> KeyboardService::getKeys()
{
    return q->dequeue();
}
char KeyboardService::getInput()
{
    char buf = 0;
    struct termios old;
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}
void KeyboardService::dispose() {
    (*disposed) = true;
}