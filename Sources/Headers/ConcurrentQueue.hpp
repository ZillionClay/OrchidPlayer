//
// Created by 11312 on 2023/11/30.
//

#ifndef ORCHIDPLAYER_CONCURRENTQUEUE_HPP
#define ORCHIDPLAYER_CONCURRENTQUEUE_HPP

#include <queue>
#include <mutex>

template<typename T>
class ConcurrentQueue
{
private:
    std::queue<T> m_Queue;
    std::mutex m_Mutex;

public:
    ConcurrentQueue() = default;
    ~ConcurrentQueue() = default;

    template<typename U>
    inline auto enqueue(U&& element)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.push(std::forward<U>(element));
    }

    template<typename ...Args>
    inline auto emplace(Args&& ...args)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.emplace(std::forward<Args>(args)...);
    }

    inline bool dequeue(T& out)
    {
        if(m_Queue.empty())
        { return false; }
        std::lock_guard<std::mutex> lock(m_Mutex);
        if(m_Queue.empty())
        { return false; }
        out = m_Queue.front();
        m_Queue.pop();
        return true;
    }
};

#endif //ORCHIDPLAYER_CONCURRENTQUEUE_HPP
