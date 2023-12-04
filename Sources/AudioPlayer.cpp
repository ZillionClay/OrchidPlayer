//
// Created by 11312 on 2023/11/28.
//

#include <future>
#include "AudioPlayer.hpp"
#include "AudioReSampler.hpp"

bool AudioPlayer::Open(const DecodeContext &decoder, int32_t outSampleRate)
{
    auto func = [this, &decoder, outSampleRate]()
    {
        this->m_Decoder = decoder;
        this->m_OutSampleRate = outSampleRate;
        this->m_Progress = 0;
        this->m_Duration = (double)this->m_Decoder.m_pAvFormatCtx->duration/AV_TIME_BASE;
        this->m_Loaded = true;
    };
    return m_Controller.Open(func);
}

bool AudioPlayer::Open(std::string_view url, int32_t outSampleRate)
{
    auto func = [this, url, outSampleRate]()
    {
        this->m_Decoder.Load(url);
        this->m_OutSampleRate = outSampleRate;
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

    Consumer consumer;
    while(m_ConsumerQue.dequeue(consumer))
    {
        consumer(*this);
    }

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
        m_Source.PushData(pDstFrame->data[0], pDstFrame->nb_samples * 8);
        auto timeBase = m_Decoder.m_pAvFormatCtx->streams[pPacket->stream_index]->time_base;
        m_Progress = (double)pPacket->pts * av_q2d(timeBase);
        av_frame_free(&pSrcFrame);
        av_frame_free(&pDstFrame);
    }

    av_packet_free(&pPacket);
}

void AudioPlayer::ExecuteOpen()
{
    AVChannelLayout outChnlLayout;
    av_channel_layout_default(&outChnlLayout, 2);

    m_Resampler = AudioReSampler(AV_SAMPLE_FMT_FLT, m_Decoder.m_pAudDecodeCtx->sample_fmt,
                                         outChnlLayout, m_Decoder.m_pAudDecodeCtx->ch_layout,
                                         m_OutSampleRate, m_Decoder.m_pAudDecodeCtx->sample_rate);

    m_Source = OpenALSource(8, AL_FORMAT_STEREO_FLOAT32, m_OutSampleRate);
}

void AudioPlayer::ExecuteStart()
{
    m_Source.Play();
}

void AudioPlayer::ExecuteStop()
{
    m_Source.Pause();
}

void AudioPlayer::ExecuteClose()
{
    m_Resampler.Close();
    m_Source.Close();
    m_Decoder.Close();
    m_Progress = NAN;
    m_Duration = NAN;
    this->m_Loaded = false;
}

double AudioPlayer::Tell()
{
    return (double)m_Progress;
}

double AudioPlayer::GetDuration()
{
    return m_Duration;
}

void AudioPlayer::Locate(double second)
{
    m_ConsumerQue.enqueue(  [second](AudioPlayer& player)
    {
        if(player.m_Decoder.m_pAvFormatCtx == nullptr) return;
        avformat_seek_file(player.m_Decoder.m_pAvFormatCtx, -1, INT_MIN, static_cast<int>(second*AV_TIME_BASE), INT_MAX, 0);
    });
}

AudioPlayer::AudioPlayer()
{
    m_Controller.SetRecurrent(this);
}
