//
// Created by 11312 on 2023/11/25.
//

#ifndef ORCHIDPLAYER_AUDIORESAMPLER_HPP
#define ORCHIDPLAYER_AUDIORESAMPLER_HPP

#include <vector>

extern "C"
{
#include "libavutil/avconfig.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class AudioReSampler
{
public:

    AVSampleFormat m_eOutSmplFmt;
    AVChannelLayout m_outChnlLayout;
    int32_t m_nOutSmplRate;

    AVSampleFormat m_eInSmplFmt;
    AVChannelLayout m_inChnlLayout;
    int32_t m_nInSmplRate;

    SwrContext* m_pSwrCtx          = nullptr; // 重采样器上下文

    AudioReSampler() = default;
    AudioReSampler(const AVSampleFormat&    eOutSmplFmt,       // 输出音频格式
                  const AVSampleFormat&     eInSmplFmt,        // 输入音频格式
                  const AVChannelLayout&    outChnlLayout,     // 输出音频通道布局
                  const AVChannelLayout&    inChnlLayout,      // 输入音频通道布局
                  int32_t                   nOutSmplRate,      // 输出音频采样率
                  int32_t                   nInSmplRate        // 输入音频采样率
                  );

    ~AudioReSampler() = default;

    void Close();

    inline int64_t GetSwrDelay() const
    {
        return swr_get_delay(m_pSwrCtx, m_nInSmplRate);
    }

    int64_t CovertFrame(AVFrame** pOutFrame, const AVFrame* pInFrame= nullptr);

    // int64_t CovertData(uint8_t** outDataBuf, const uint8_t** inDataBuf, size_t inSize);

};


#endif //ORCHIDPLAYER_AUDIORESAMPLER_HPP
