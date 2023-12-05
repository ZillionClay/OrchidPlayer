//
// Created by 11312 on 2023/11/28.
//

#include <future>
#include "AudioPlayer.hpp"
#include "AudioReSampler.hpp"

bool AudioPlayer::Open(const DecodeContext &decoder)
{
    auto func = [this, &decoder]()
    {
        this->m_Decoder = decoder;
        this->m_SampleRate = m_Decoder.m_pAudDecodeCtx->sample_rate;
        this->m_SampleFotmat = AV_SAMPLE_FMT_FLT;
        this->m_Progress = 0;
        this->m_Duration = (double)this->m_Decoder.m_pAvFormatCtx->duration/AV_TIME_BASE;
        this->m_Loaded = true;
    };
    return m_Controller.Open(func);
}

bool AudioPlayer::Open(std::string_view url)
{
    auto func = [this, url]()
    {
        this->m_Decoder.Load(url);
        this->m_SampleRate = m_Decoder.m_pAudDecodeCtx->sample_rate;
        this->m_SampleFotmat = AV_SAMPLE_FMT_FLT;
        this->m_Progress = 0;
        this->m_Duration = (double)this->m_Decoder.m_pAvFormatCtx->duration/AV_TIME_BASE;
        this->m_Loaded = true;
    };
    return m_Controller.Open(func);
}

bool AudioPlayer::Start()
{
    return m_Controller.Start();
}

bool AudioPlayer::Stop()
{
    return m_Controller.Stop();
}

bool AudioPlayer::Close()
{
    return m_Controller.Close();
}

void AudioPlayer::ExecuteRun()
{
    AVFrame *pSrcFrame = nullptr;
    AVFrame *pDstFrame = nullptr;
    AVPacket* pPacket = av_packet_alloc();

    int res = av_read_frame(m_Decoder.m_pAvFormatCtx, pPacket);
    m_Ended = false;
    if (res == AVERROR_EOF)  // 正常读取到文件尾部退出
    {
        m_Ended = true;
        av_packet_free(&pPacket);
        Util::LOGDFMT("<ExecuteRun> reached media file end\n");
        if(m_AutoLoop)
        {
            Locate(0);
        }
        else
        {
            Stop();
        }
        return;

    }
    else if (res < 0) // 其他小于0的返回值是数据包读取错误
    {
        av_packet_free(&pPacket);
        Util::LOGEFMT("<ExecuteRun> fail av_read_frame(), res=%d\n", res);
        Close();
        return;
    }


    if (pPacket && pPacket->stream_index == m_Decoder.GetAudStreamIndex())
    {

        int decode_res = m_Decoder.DecodeAudioFrame(pPacket, &pSrcFrame);
        if(decode_res != 0)
            return;
        m_Resampler.CovertFrame(&pDstFrame, pSrcFrame);

        int sampleSize = GetSampleSize(static_cast<AVSampleFormat>(pDstFrame->format), pDstFrame->ch_layout.nb_channels);
        m_Source.PushData(pDstFrame->data[0], pDstFrame->nb_samples * sampleSize);

        auto timeBase = m_Decoder.m_pAvFormatCtx->streams[pPacket->stream_index]->time_base;
        m_Progress = (double)pPacket->pts * av_q2d(timeBase);

        av_frame_free(&pSrcFrame);
        av_frame_free(&pDstFrame);
    }

    av_packet_free(&pPacket);

    Consumer consumer;
    while(m_ConsumerQue.dequeue(consumer))
    {
        consumer(*this);
    }

    for(auto& listener: m_Listeners.Run)
    {
        listener(*this);
    }

}

void AudioPlayer::ExecuteOpen()
{
    AVChannelLayout outChnlLayout;
    m_Channels = m_Decoder.m_pAudDecodeCtx->ch_layout.nb_channels;
    av_channel_layout_default(&outChnlLayout, m_Channels);

    m_Resampler = AudioReSampler(m_SampleFotmat, m_Decoder.m_pAudDecodeCtx->sample_fmt,
                                 outChnlLayout, m_Decoder.m_pAudDecodeCtx->ch_layout,
                                 m_SampleRate, m_Decoder.m_pAudDecodeCtx->sample_rate);

    m_Source = OpenALSource(8, GetALFormat(m_SampleFotmat, m_Channels), m_SampleRate);

    for(auto& listener: m_Listeners.Open)
    {
        listener(*this);
    }
}

void AudioPlayer::ExecuteStart()
{
    m_Source.Play();
    for(auto& listener: m_Listeners.Start)
    {
        listener(*this);
    }
}

void AudioPlayer::ExecuteStop()
{
    m_Source.Pause();
    for(auto& listener: m_Listeners.Stop)
    {
        listener(*this);
    }
}

void AudioPlayer::ExecuteClose()
{
    m_Resampler.Close();
    m_Source.Close();
    m_Decoder.Close();
    this->m_Loaded = false;
    m_Progress = NAN;
    m_Duration = NAN;
    for(auto& listener: m_Listeners.Close)
    {
        listener(*this);
    }
}

double AudioPlayer::Tell()
{
    return (double)m_Progress;
}

double AudioPlayer::GetDuration()
{
    return m_Duration;
}

void AudioPlayer::Locate(double second, const AudioPlayer::Callback &callback)
{
    m_ConsumerQue.enqueue(  [this, second, callback](AudioPlayer& player)
    {
        if(player.m_Decoder.m_pAvFormatCtx == nullptr) return;
        avformat_seek_file(player.m_Decoder.m_pAvFormatCtx, -1, INT_MIN, static_cast<int>(second*AV_TIME_BASE), INT_MAX, 0);
        m_Progress = second;
        callback(player);
    });
}

AudioPlayer::AudioPlayer()
{
    m_Controller.SetRecurrent(this);
}

void AudioPlayer::SetVolume(float vol)
{
    m_ConsumerQue.enqueue(  [vol](AudioPlayer& player)
    {
        player.m_Source.SetVolume(vol);
    });

}

ALint AudioPlayer::GetALFormat(AVSampleFormat format, int channels) noexcept(false)
{
    ALint res = AL_FORMAT_MONO8;
    switch (format)
    {
        case AV_SAMPLE_FMT_U8:
            if(channels == 1)
                res = AL_FORMAT_MONO8;
            else if(channels == 2)
                res = AL_FORMAT_STEREO8;
            else throw std::runtime_error("unknown format");
            break;
        case AV_SAMPLE_FMT_S16:
            if(channels == 1)
                res = AL_FORMAT_MONO16;
            else if(channels == 2)
                res = AL_FORMAT_STEREO16;
            else throw std::runtime_error("unknown format");
            break;
        case AV_SAMPLE_FMT_FLT:
            if(channels == 1)
                res = AL_FORMAT_MONO_FLOAT32;
            else if(channels == 2)
                res = AL_FORMAT_STEREO_FLOAT32;
            else throw std::runtime_error("unknown format");
            break;
        case AV_SAMPLE_FMT_DBL:
            if(channels == 1)
                res = AL_FORMAT_MONO_DOUBLE_EXT;
            else if(channels == 2)
                res = AL_FORMAT_STEREO_DOUBLE_EXT;
            else throw std::runtime_error("unknown format");
            break;
        default:
            throw std::runtime_error("unknown format");
    }
    return res;
}

void AudioPlayer::SetSampleRate(int sampleRate)
{
    m_ConsumerQue.enqueue([sampleRate](AudioPlayer& player)
    {
        player.m_SampleRate = sampleRate;
        player.m_Resampler.SetOutSampleRate(sampleRate);
        player.m_Source.SetSampleRate(sampleRate);
    });
}

void AudioPlayer::SetFormat(AVSampleFormat format)
{
    m_ConsumerQue.enqueue([format](AudioPlayer& player)
    {
        player.m_SampleFotmat = format;
        player.m_Resampler.SetOutSampleFormat(format);
        player.m_Source.SetSampleFormat(GetALFormat(format, player.m_Channels));
    });
}

ALint AudioPlayer::GetSampleSize(AVSampleFormat format, int channels) noexcept(false)
{
    ALint bits = 8;
    switch (format)
    {
        case AV_SAMPLE_FMT_U8:
            if(channels == 1)
                bits = 8;
            else if(channels == 2)
                bits = 8*2;
            else throw std::runtime_error("unknown format");
            break;
        case AV_SAMPLE_FMT_S16:
            if(channels == 1)
                bits = 16;
            else if(channels == 2)
                bits = 16*2;
            else throw std::runtime_error("unknown format");
            break;
        case AV_SAMPLE_FMT_FLT:
            if(channels == 1)
                bits = 32;
            else if(channels == 2)
                bits = 32*2;
            else throw std::runtime_error("unknown format");
            break;
        case AV_SAMPLE_FMT_DBL:
            if(channels == 1)
                bits = 64;
            else if(channels == 2)
                bits = 64*2;
            else throw std::runtime_error("unknown format");
            break;
        default:
            throw std::runtime_error("unknown format");
    }
    return bits>>3;
}

AVSampleFormat AudioPlayer::NarrowFormat(AVSampleFormat format)
{
    switch (format)
    {
        case AV_SAMPLE_FMT_U8:      return AV_SAMPLE_FMT_U8;
        case AV_SAMPLE_FMT_U8P:     return AV_SAMPLE_FMT_U8;
        case AV_SAMPLE_FMT_S16:     return AV_SAMPLE_FMT_S16;
        case AV_SAMPLE_FMT_S16P:    return AV_SAMPLE_FMT_S16;
        case AV_SAMPLE_FMT_FLT:     return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_FLTP:    return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_S32:     return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_S32P:    return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_S64:     return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_S64P:    return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_DBL:     return AV_SAMPLE_FMT_DBL;
        case AV_SAMPLE_FMT_DBLP:    return AV_SAMPLE_FMT_DBL;
        default:                    return AV_SAMPLE_FMT_FLT;
    }
}

