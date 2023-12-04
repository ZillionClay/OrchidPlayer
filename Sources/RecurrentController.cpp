//
// Created by 11312 on 2023/12/1.
//

#include "RecurrentController.hpp"

void RecurrentController::Worker(RecurrentController &controller)
{
    std::lock_guard<std::mutex> recurrentLock(controller.m_RecurrentMutex);
    auto& recurrent = *controller.m_Recurrent;
    recurrent.ExecuteOpen();
    bool running = false;
    for(;;)
    {
        switch (controller.m_State)
        {
            case State::RUNNING:
                if(not running)
                {
                    recurrent.ExecuteStart();
                    running = true;
                }
                else
                {
                    recurrent.ExecuteRun();
                }
                break;
            case State::STOPPED:
                if(running)
                {
                    recurrent.ExecuteStop();
                    running = false;
                }
                else
                {
                    std::unique_lock<std::mutex> PauseLock(controller.m_PauseMutex);
                    controller.m_PauseCV.wait(PauseLock);
                }
                break;
            case State::CLOSED:
                if(running)
                {
                    recurrent.ExecuteStop();
                    running = false;
                }
                recurrent.ExecuteClose();
                return;
        }

    }

}

bool RecurrentController::Open(const Runnable& func)
{
    if(m_State != State::CLOSED)
    {
        return false;
    }
    std::lock_guard<std::mutex> lock(m_OperationMutex);
    if(m_State != State::CLOSED)
    {
        return false;
    }
    func();
    m_State = State::STOPPED;
    m_WorkerThread = new std::thread(Worker, std::ref(*this));
    return true;
}

bool RecurrentController::Start(const Runnable& func)
{
    if(m_State != State::STOPPED)
    {
        return false;
    }
    std::lock_guard<std::mutex> lock(m_OperationMutex);
    if(m_State != State::STOPPED)
    {
        return false;
    }
    func();
    m_State = State::RUNNING;
    m_PauseCV.notify_one();
    return true;
}

bool RecurrentController::Stop(const Runnable& func)
{
    if(m_State != State::RUNNING)
    {
        return false;
    }
    std::lock_guard<std::mutex> lock(m_OperationMutex);
    if(m_State != State::RUNNING)
    {
        return false;
    }
    func();
    m_State = State::STOPPED;
    return true;
}

bool RecurrentController::Close(const Runnable& func)
{
    if(m_State == State::CLOSED)
    {
        return false;
    }
    std::lock_guard<std::mutex> lock(m_OperationMutex);
    if(m_State == State::CLOSED)
    {
        return false;
    }
    func();
    m_State = State::CLOSED;
    m_PauseCV.notify_one();
    m_WorkerThread->join();
    m_WorkerThread = nullptr;
    return true;
}

void RecurrentController::SetRecurrent(IRecurrent *recurrent)
{
    m_Recurrent = recurrent;
}

IRecurrent *RecurrentController::getMRecurrent() const
{
    return m_Recurrent;
}
