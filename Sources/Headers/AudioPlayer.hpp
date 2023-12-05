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
    using Listener = std::function<void(AudioPlayer&)>;
    using Callback = std::function<void(AudioPlayer&)>;


    struct PlayerListeners
    {
        std::vector<Listener> Open;
        std::vector<Listener> Start;
        std::vector<Listener> Run;
        std::vector<Listener> Stop;
        std::vector<Listener> Close;
    };

    PlayerListeners m_Listeners;

private:
    OpenALSource m_Source;
    DecodeContext m_Decoder;
    AudioReSampler m_Resampler;
    ConcurrentQueue<Consumer> m_ConsumerQue;
    RecurrentController m_Controller;
    int32_t m_SampleRate = 44100;
    int m_Channels = 2;
    AVSampleFormat m_SampleFotmat = AV_SAMPLE_FMT_FLT;
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

    static ALint GetALFormat(AVSampleFormat format, int channels) noexcept(false);
    static ALint GetSampleSize(AVSampleFormat format, int channels) noexcept(false);
    static AVSampleFormat NarrowFormat(AVSampleFormat format);

    int32_t GetSampleRate() const& {return m_SampleRate;}
    int GetNumChannel() const& {return m_Channels;}
    AVSampleFormat GetFormat() const& {return m_SampleFotmat;}

    std::atomic<bool> m_AutoLoop = false;

    AudioPlayer();
    ~AudioPlayer() = default;

    RecurrentController::State GetState() const& {return m_Controller.GetState();}
    bool IsEnd() const& {return m_Ended;}
    bool Loaded() const& {return m_Loaded;}

    void SetVolume(float vol);
    void SetSampleRate(int sampleRate);
    void SetFormat(AVSampleFormat format);

    bool Open(std::string_view url);
    bool Open(const DecodeContext &decoder);

    bool Start();

    bool Stop();

    bool Close();

    double GetDuration();

    double Tell();

    void Locate(double second, const Callback& callback = [](AudioPlayer&){});
};


#endif //ORCHIDPLAYER_AUDIOPLAYER_HPP
