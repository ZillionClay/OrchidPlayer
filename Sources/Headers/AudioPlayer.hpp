//
// Created by 11312 on 2023/11/28.
//

#ifndef ORCHIDPLAYER_AUDIOPLAYER_HPP
#define ORCHIDPLAYER_AUDIOPLAYER_HPP

#include "OpenALContext.hpp"
#include "OpenALSource.hpp"
#include "DecodeContext.hpp"
#include <cassert>
#include <thread>
#include <condition_variable>
#include <functional>
#include "ConcurrentQueue.hpp"
#include "AudioReSampler.hpp"
#include "IRecurrent.hpp"
#include "RecurrentController.hpp"


class AudioPlayer : public IRecurrent
{
public:
    using Consumer = std::function<void(AudioPlayer&)>;

private:

    OpenALSource m_Source;
    DecodeContext m_Decoder;
    AudioReSampler m_Resampler;
    ConcurrentQueue<Consumer> m_ConsumerQue;
    RecurrentController m_Controller;
    int32_t m_OutSampleRate;
    double m_Progress = NAN;
    double m_Duration = NAN;
    bool m_Ended = false;
    bool m_Loaded = false;

    void ExecuteRun() override;
    void ExecuteOpen() override;
    void ExecuteStart() override;
    void ExecuteStop() override;
    void ExecuteClose() override;

public:

    std::atomic<bool> m_AutoLoop = false;

    AudioPlayer();
    ~AudioPlayer() = default;

    RecurrentController::State GetState() const& {return m_Controller.GetState();}
    bool IsEnd() const& {return m_Ended;}
    bool Loaded() const& {return m_Loaded;}

    bool Open(std::string_view url, int32_t outSampleRate = 44100);
    bool Open(const DecodeContext &decoder, int32_t outSampleRate = 44100);

    bool Start();

    bool Stop();

    bool Close();

    double GetDuration();

    double Tell();

    void Locate(double second);
};


#endif //ORCHIDPLAYER_AUDIOPLAYER_HPP
