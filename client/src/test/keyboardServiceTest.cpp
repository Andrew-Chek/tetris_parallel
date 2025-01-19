#include <iostream>
#include <thread>
#include <cstdlib>

#include "../lib/keyboardService.cpp"
#include "../lib/common/sharedQueue.cpp"

int main()
{
    KeyboardService keyboardService = KeyboardService();
    std::thread t1(&KeyboardService::run, keyboardService);

    for (;;)
    {
        std::queue<char> keys = keyboardService.getKeys();
        while (!keys.empty()) {
            std::cout << keys.front() << "\n";
            keys.pop();
        }
        usleep(16 * 1e4); // us
    }

    t1.join();
}