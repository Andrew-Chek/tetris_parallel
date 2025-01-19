#ifndef SHARED_QUEUE_HPP
#define SHARED_QUEUE_HPP

#include <queue>
#include <mutex>

template <class T>
class SharedQueue
{
public:
    SharedQueue();
    void enqueue(const T data);
    std::queue<T> dequeue();
private:
    std::queue<T> *q;
    std::mutex *mtx;
};


#endif
