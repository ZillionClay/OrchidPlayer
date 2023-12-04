//
// Created by 11312 on 2023/12/1.
//

#ifndef ORCHIDPLAYER_RECURRENTCONTROLLER_HPP
#define ORCHIDPLAYER_RECURRENTCONTROLLER_HPP

#include "IRecurrent.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class RecurrentController
{
public:

    enum State
    {
        CLOSED, STOPPED, RUNNING
    };

    explicit RecurrentController() = default;
    void SetRecurrent(IRecurrent* recurrent);
    IRecurrent *getMRecurrent() const;

    using Runnable = std::function<void()>;

    bool Open(const Runnable& func = [](){});
    bool Start(const Runnable& func = [](){});
    bool Stop(const Runnable& func = [](){});
    bool Close(const Runnable& func = [](){});
    inline State GetState() const& {return m_State;}

private:
    IRecurrent* m_Recurrent;
    State m_State = CLOSED;
    std::mutex m_OperationMutex;
    std::mutex m_RecurrentMutex;
    std::mutex m_PauseMutex;
    std::condition_variable m_PauseCV;
    std::thread* m_WorkerThread = nullptr;

    static void Worker(RecurrentController& controller);

};


#endif //ORCHIDPLAYER_RECURRENTCONTROLLER_HPP
