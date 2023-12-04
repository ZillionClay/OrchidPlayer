//
// Created by 11312 on 2023/11/29.
//

#include "DecodeContext.hpp"
#include "util.hpp"

void DecodeContext::Load(std::string_view url)
{
    Close();
    AVCodec *pVidDecoder = nullptr;
    AVCodec *pAudDecoder = nullptr;
    AVStream *pVidStream = nullptr;
    AVStream *pAudStream = nullptr;
    int res = 0;

    // 打开媒体文件
    res = avformat_open_input(&m_pAvFormatCtx, url.data(), nullptr, nullptr);
    if (m_pAvFormatCtx == nullptr)
    {
        Util::LOGEFMT("<Load> fail avformat_open_input(), res=%d\n", res);
        throw res;
    }

    // 查找所有媒体流信息
    res = avformat_find_stream_info(m_pAvFormatCtx, nullptr);
    if (res == AVERROR_EOF)
    {
        Util::LOGDFMT("<Load> reached to file end\n");
        Close();
        throw res;
    }

    // 遍历所有的媒体流信息
    for (unsigned int i = 0; i < m_pAvFormatCtx->nb_streams; ++i)
    {
        AVStream *pAvStream = m_pAvFormatCtx->streams[i];
        if (pAvStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if ((pAvStream->codecpar->width <= 0) || (pAvStream->codecpar->height <= 0))
            {
                Util::LOGEFMT("<Load> invalid resolution, streamIndex=%d\n", i);
                continue;
            }

            pVidDecoder = (AVCodec *) avcodec_find_decoder(pAvStream->codecpar->codec_id);  // 找到视频解码器
            if (pVidDecoder == nullptr)
            {
                Util::LOGEFMT("%s", "<Load> can not find video codec\n");
                continue;
            }
            pVidStream = pAvStream;

            m_nVidStreamIndex = static_cast<uint32_t>(i);
            Util::LOGDFMT("<Load> pxlFmt=%d, frameSize=%d*%d\n",
                          (int) pAvStream->codecpar->format,
                          pAvStream->codecpar->width,
                          pAvStream->codecpar->height);

        }
        else if (pAvStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            if ((pAvStream->codecpar->ch_layout.nb_channels <= 0) || (pAvStream->codecpar->sample_rate <= 0))
            {
                Util::LOGEFMT("<Load> invalid resolution, streamIndex=%d\n", i);
                continue;
            }

            pAudDecoder = (AVCodec *) avcodec_find_decoder(pAvStream->codecpar->codec_id);  // 找到音频解码器
            if (pAudDecoder == nullptr)
            {
                Util::LOGEFMT("<Load> can not find Audio codec\n");
                continue;
            }

            pAudStream = pAvStream;

            m_nAudStreamIndex = static_cast<uint32_t>(i);
            Util::LOGDFMT("<Load> sample_fmt=%d, sampleRate=%d, channels=%d, order of ch_layout=%d\n",
                          (int) pAvStream->codecpar->format,
                          pAvStream->codecpar->sample_rate,
                          pAvStream->codecpar->ch_layout.nb_channels,
                          pAvStream->codecpar->ch_layout.order);
        }
    }
    if (pVidDecoder == nullptr && pAudDecoder == nullptr)
    {
        Close();
        throw std::runtime_error("<Load> can not find video or audio stream\n");
    }

    // 创建视频解码器并且打开
    if (pVidDecoder != nullptr)
    {
        m_pVidDecodeCtx = avcodec_alloc_context3(pVidDecoder);
        if (m_pVidDecodeCtx == nullptr)
        {
            Util::LOGEFMT("<Load> fail to video avcodec_alloc_context3()\n");
            Close();
        }
        avcodec_parameters_to_context(m_pVidDecodeCtx, m_pAvFormatCtx->streams[m_nVidStreamIndex]->codecpar);

        res = avcodec_open2(m_pVidDecodeCtx, nullptr, nullptr);
        if (res != 0)
        {
            Util::LOGEFMT("<Load> fail to video avcodec_open2(), res=%d\n", res);
            Close();
        }
        else
            m_pVidDecodeCtx->pkt_timebase = pVidStream->time_base;
    }

    // 创建音频解码器并且打开
    if (pAudDecoder != nullptr)
    {
        m_pAudDecodeCtx = avcodec_alloc_context3(pAudDecoder);
        if (m_pAudDecodeCtx == nullptr)
        {
            Util::LOGEFMT("<Load> fail to audio avcodec_alloc_context3()\n");
            Close();
        }

        avcodec_parameters_to_context(m_pAudDecodeCtx, m_pAvFormatCtx->streams[m_nAudStreamIndex]->codecpar);

        res = avcodec_open2(m_pAudDecodeCtx, nullptr, nullptr);
        if (res != 0)
        {
            Util::LOGEFMT("<Load> fail to audio avcodec_open2(), res=%d\n", res);
            Close();
        }
        else
            m_pAudDecodeCtx->pkt_timebase = pAudStream->time_base;
    }
}

void DecodeContext::Close()
{
    // 关闭媒体文件解析
    if (m_pAvFormatCtx != nullptr)
    {
        avformat_close_input(&m_pAvFormatCtx);
        m_pAvFormatCtx = nullptr;
    }

    // 关闭视频解码器
    if (m_pVidDecodeCtx != nullptr)
    {
        avcodec_close(m_pVidDecodeCtx);
        avcodec_free_context(&m_pVidDecodeCtx);
        m_pVidDecodeCtx = nullptr;
    }

    // 关闭音频解码器
    if (m_pAudDecodeCtx != nullptr)
    {
        avcodec_close(m_pAudDecodeCtx);
        avcodec_free_context(&m_pAudDecodeCtx);
        m_pAudDecodeCtx = nullptr;
    }
}

int DecodeContext::DecodePktToFrame(AVCodecContext* pDecoderCtx,
                              const AVPacket* pInPacket,
                              AVFrame** ppOutFrame)
{
    AVFrame*  pOutFrame = nullptr;
    int       res       = 0;

    res = avcodec_send_packet(pDecoderCtx, pInPacket);
    if(res == AVERROR(EAGAIN)) // 缓冲区已满，要从内部缓冲区读取解码后的音视频帧
    {
        Util::LOGDFMT("<DecodePktToFrame> avcodec_send_frame() EAGAIN\n");
    }
    else if (res == AVERROR_EOF) // 数据包送入结束不再送入,但是可以继续可以从内部缓冲区读取解码后的音视频帧
    {
        Util::LOGDFMT("<DecodePktToFrame> avcodec_send_frame() AVERROR_EOF\n");
    }
    else if (res < 0)  // 送入输入数据包失败
    {
        Util::LOGEFMT("<DecodePktToFrame> fail to avcodec_send_frame(), res=%d\n", res);
        return res;
    }

    pOutFrame = av_frame_alloc();
    res       = avcodec_receive_frame(pDecoderCtx, pOutFrame);

    if (res == AVERROR(EAGAIN)) // 当前这次没有解码没有音视频帧输出,需要送入更多的数据包
    {
        Util::LOGDFMT("<DecodePktToFrame> no data output\n");
        av_frame_free(&pOutFrame);
        (*ppOutFrame) = nullptr;
        return AVERROR(EAGAIN);
    }
    else if (res == AVERROR_EOF) // 解码缓冲区已经刷新完成,后续不再有音视频帧输出
    {
        Util::LOGDFMT("<DecodePktToFrame> avcodec_receive_packet() EOF\n");
        av_frame_free(&pOutFrame);
        (*ppOutFrame) = nullptr;
        return AVERROR_EOF;
    }
    else if (res < 0)
    {
        Util::LOGEFMT("<DecodePktToFrame> fail to avcodec_receive_packet(), res=%d\n", res);
        av_frame_free(&pOutFrame);
        (*ppOutFrame) = nullptr;
        return res;
    }

    (*ppOutFrame) = pOutFrame;
    return 0;
}

int DecodeContext::DecodeAudioFrame(const AVPacket *pInPacket, AVFrame** ppOutFrame) const
{
    return DecodePktToFrame(this->m_pAudDecodeCtx, pInPacket, ppOutFrame);
}

int DecodeContext::DecodeVideoFrame(const AVPacket *pInPacket, AVFrame** ppOutFrame) const
{
    return DecodePktToFrame(this->m_pVidDecodeCtx, pInPacket, ppOutFrame);
}

void DecodeContext::Locate(int streamIndex, int frame)
{
    avformat_seek_file(m_pAvFormatCtx, streamIndex, 0, frame, INT_MAX, AVSEEK_FLAG_FRAME);
}

