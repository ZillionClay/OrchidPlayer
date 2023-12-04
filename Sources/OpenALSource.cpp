//
// Created by 11312 on 2023/11/25.
//
#include <memory>
#include "OpenALSource.hpp"

OpenALSource::OpenALSource(int numBuffer, ALint format, ALint sampleRate)
{
    m_numBuffers = numBuffer;
    Init(format, sampleRate);
}

OpenALSource::OpenALSource(int numBuffer)
{
    m_numBuffers = numBuffer;
    Init();
}

void OpenALSource::Init(ALint format, ALint sampleRate)

{
    uint32_t *arr = new uint32_t[m_numBuffers];
    alGenBuffers(m_numBuffers, arr);
    for(int i = 0; i < m_numBuffers; ++i)
    {
        m_bidQue.push(arr[i]);
    }
    delete[] arr;
    alGenSources(1, &m_alSourcei);
    m_Format = format;
    m_SampleRate = sampleRate;
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
    alDeleteSources(1, &m_alSourcei);
    while(!m_bidQue.empty())
    {
        OpenALBuffer buf(m_bidQue.front());
        m_bidQue.pop();
        buf.DeleteBuf();
    }
}

void OpenALSource::SetFormat(ALint format, ALint sampleRate)
{
    Close();
    m_Format = format;
    m_SampleRate = sampleRate;
    Init(m_Format, m_SampleRate);
}

void OpenALSource::SetVolume(float vol)
{
    alSourcef(m_alSourcei, AL_GAIN, vol);
}

void OpenALSource::SetSampleFormat(ALint sampleFormat)
{
    Close();
    m_Format = sampleFormat;
    Init(m_Format, m_SampleRate);
}

void OpenALSource::SetSampleRate(ALint sampleRate)
{
    Close();
    m_SampleRate = sampleRate;
    Init(m_Format, m_SampleRate);
}

