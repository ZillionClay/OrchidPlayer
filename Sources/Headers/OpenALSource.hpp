//
// Created by 11312 on 2023/11/25.
//

#ifndef ORCHIDPLAYER_OPENALSOURCE_HPP
#define ORCHIDPLAYER_OPENALSOURCE_HPP

#include "OpenALBuffers.hpp"
#include "OpenAL\al.h"
#include "OpenAL\alc.h"
#include "util.hpp"
#include <queue>
#include <thread>

class OpenALSource
{
private:
    ALint m_Format;
    ALint m_SampleRate;
    uint32_t m_alSourcei;
    int m_numBuffers;
    std::queue<uint32_t> m_bidQue;

    int GetNumProcessed() const;
    int GetNumQueued() const;
    int Flush();

public:

    OpenALSource() = default;
    explicit OpenALSource(int numBuffer);
    explicit OpenALSource(int numBuffer, ALint format, ALint sampleRate);
    ~OpenALSource() = default;

    void SetFormat(ALint format, ALint sampleRate);

    OpenALSource& PushData(const uint8_t* buf, size_t size);

    OpenALSource& operator<<(const std::vector<uint8_t>& buf)
    {
        PushData(buf.data(), buf.size());
        return *this;
    }

    void Play();

    void Pause();

    void Stop();

    void Close();
};


#endif //ORCHIDPLAYER_OPENALSOURCE_HPP
