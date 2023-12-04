//
// Created by 11312 on 2023/11/29.
//

#ifndef ORCHIDPLAYER_DECODECONTEXT_HPP
#define ORCHIDPLAYER_DECODECONTEXT_HPP

#include <string_view>
#include <queue>
#include <memory>
#include <thread>

extern "C"
{
#include "libavutil/avconfig.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class DecodeContext
{
private:

    uint32_t                m_nVidStreamIndex   = -1;       // 视频流索引值
    uint32_t                m_nAudStreamIndex   = -1;       // 音频流索引值

    // std::thread m_decodeThread;

    using FFLAG = int32_t;

public:

    AVFormatContext*        m_pAvFormatCtx      = nullptr;  // 流文件解析上下文
    AVCodecContext*         m_pVidDecodeCtx     = nullptr;  // 视频解码器上下文
    AVCodecContext*         m_pAudDecodeCtx     = nullptr;  // 音频解码器上下文

    DecodeContext() = default;
    ~DecodeContext() = default;

    inline uint32_t GetVidStreamIndex() const&
    {
        return m_nVidStreamIndex;
    }
    inline uint32_t GetAudStreamIndex() const&
    {
        return m_nAudStreamIndex;
    }

    void Load(std::string_view url);

    void Close();

    void Locate(int streamIndex, int frame);

    int DecodeAudioFrame(const AVPacket *pInPacket, AVFrame **ppOutFrame) const;

    int DecodeVideoFrame(const AVPacket *pInPacket, AVFrame **ppOutFrame) const;

    static int DecodePktToFrame(AVCodecContext *pDecoderCtx, const AVPacket *pInPacket, AVFrame **ppOutFrame);
};

#endif //ORCHIDPLAYER_DECODECONTEXT_HPP
