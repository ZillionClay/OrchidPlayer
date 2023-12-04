//
// Created by 11312 on 2023/11/25.
//
#include <memory>
#include "OpenALSource.hpp"

OpenALSource::OpenALSource(int numBuffer, ALint format, ALint sampleRate)
{
    m_numBuffers = numBuffer;
    uint32_t *arr = new uint32_t[numBuffer];
    alGenBuffers(numBuffer, arr);
    for(int i = 0; i < numBuffer; ++i)
    {
        m_bidQue.push(arr[i]);
    }
    alGenSources(1, &m_alSourcei);
    m_Format = format;
    m_SampleRate = sampleRate;
}

OpenALSource::OpenALSource(int numBuffer)
{
    m_numBuffers = numBuffer;
    uint32_t *arr = new uint32_t[numBuffer];
    alGenBuffers(numBuffer, arr);
    for(int i = 0; i < numBuffer; ++i)
    {
        m_bidQue.push(arr[i]);
    }
    alGenSources(1, &m_alSourcei);
    m_Format = AL_FORMAT_STEREO16;
    m_SampleRate = 44100;
}

OpenALSource& OpenALSource::PushData(const uint8_t *buf, size_t size)
{
    std::unique_ptr<uint32_t[]> arr(new uint32_t[m_numBuffers]);
    int processed;
    while(m_bidQue.empty())
    {
        alGetSourcei(m_alSourcei, AL_BUFFERS_PROCESSED, &processed);
        if(processed != 0)
        {
            alSourceUnqueueBuffers(m_alSourcei, processed, arr.get());
            for (int i = 0; i < processed; ++i)
            {
                m_bidQue.push(arr[i]);
            }
        }
    }

    OpenALBuffer bid(m_bidQue.front());
    m_bidQue.pop();
    bid.SetData(m_Format, buf, (int)size, m_SampleRate);
    alSourceQueueBuffers(m_alSourcei, 1, &bid.GetID());
    Play();
    return *this;
}

void OpenALSource::Play()
{
    int state;
    alGetSourcei(m_alSourcei, AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING)
        alSourcePlay(m_alSourcei);
}

void OpenALSource::Pause()
{
    int state;
    alGetSourcei(m_alSourcei, AL_SOURCE_STATE, &state);
    if(state != AL_PAUSED)
        alSourcePause(m_alSourcei);
}

void OpenALSource::Stop()
{
    int state;
    alGetSourcei(m_alSourcei, AL_SOURCE_STATE, &state);
    if(state != AL_STOPPED)
        alSourceStop(m_alSourcei);
}

int OpenALSource::GetNumProcessed() const
{
    int processed;
    alGetSourcei(m_alSourcei, AL_BUFFERS_PROCESSED, &processed);
    return processed;
}

int OpenALSource::Flush()
{
    int processed = GetNumProcessed();
    if(processed == 0) return 0;

    std::unique_ptr<uint32_t[]> arr(new uint32_t[processed]);
    alSourceUnqueueBuffers(m_alSourcei, processed, arr.get());

    for(int i = 0; i < processed; ++i)
    {
        m_bidQue.push(arr[i]);
    }

    return processed;
}

int OpenALSource::GetNumQueued() const
{
    int queued;
    alGetSourcei(m_alSourcei, AL_BUFFERS_QUEUED, &queued);
    return queued;
}

void OpenALSource::Close()
{
    if(m_alSourcei == 0) return;
    Stop();
    alSourcei(m_alSourcei, AL_BUFFER, 0);
    while(!m_bidQue.empty())
    {
        OpenALBuffer buf(m_bidQue.front());
        m_bidQue.pop();
        buf.DeleteBuf();
    }
    alDeleteSources(1, &m_alSourcei);
}

void OpenALSource::SetFormat(ALint format, ALint sampleRate)
{
    while(GetNumQueued())
    {
        Flush();
    }
    Stop();
    m_Format = format;
    m_SampleRate = sampleRate;
}
