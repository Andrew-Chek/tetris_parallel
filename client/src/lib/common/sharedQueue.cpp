#include "../../../include/lib/common/sharedQueue.hpp"

template <typename T>
SharedQueue<T>::SharedQueue()
{
    mtx = new std::mutex();
    q = new std::queue<T>();
}

template <typename T>
void SharedQueue<T>::enqueue(const T data)
{
    std::lock_guard<std::mutex> lock(*mtx);
    q->push(data);
}

template <typename T>
std::queue<T> SharedQueue<T>::dequeue()
{
    std::lock_guard<std::mutex> lock(*mtx);
    std::queue<T> ret;
    while (!q->empty())
    {
        ret.push(q->front());
        q->pop();
    }
    return ret;
}

template class SharedQueue<char>;
