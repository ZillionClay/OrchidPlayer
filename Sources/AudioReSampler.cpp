//
// Created by 11312 on 2023/11/25.
//

#include "AudioReSampler.hpp"
#include "util.hpp"
AudioReSampler::AudioReSampler(const AVSampleFormat& eOutSmplFmt, const AVSampleFormat& eInSmplFmt, const AVChannelLayout& outChnlLayout, const AVChannelLayout& inChnlLayout,
                               int32_t nOutSmplRate, int32_t nInSmplRate)
                               :m_eOutSmplFmt(eOutSmplFmt), m_eInSmplFmt(eInSmplFmt), m_outChnlLayout(outChnlLayout),
                               m_inChnlLayout(inChnlLayout), m_nOutSmplRate(nOutSmplRate), m_nInSmplRate(nInSmplRate)
{
    SwrContext* pSwrCtx = nullptr;

    swr_alloc_set_opts2(&pSwrCtx, &m_outChnlLayout, m_eOutSmplFmt, m_nOutSmplRate, &m_inChnlLayout,
                        m_eInSmplFmt, m_nInSmplRate,
                        0, nullptr  );

    if (pSwrCtx == nullptr)
    {
        Util::LOGEFMT("<AudioReSampler> fail to swr_alloc()\n");
        return;
    }

    if(swr_init(pSwrCtx) < 0)
    {
        swr_free(&pSwrCtx);
        Util::LOGEFMT("<AudioReSampler> fail to swr_init()\n");
        return;
    }

    m_pSwrCtx = pSwrCtx;

}

int64_t AudioReSampler::CovertFrame(AVFrame** ppOutFrame, const AVFrame* pInFrame)
{
    int64_t nCvtBufSamples = 0;
    if(pInFrame)
        nCvtBufSamples = av_rescale_rnd(pInFrame->nb_samples+swr_get_delay(m_pSwrCtx, m_nInSmplRate), m_nOutSmplRate, m_nInSmplRate, AV_ROUND_UP);
    else
        nCvtBufSamples = av_rescale_rnd(swr_get_delay(m_pSwrCtx, m_nInSmplRate), m_nOutSmplRate, m_nInSmplRate, AV_ROUND_UP);

    AVFrame* pOutFrame = *ppOutFrame;

    // 创建输出音频帧
    pOutFrame                 = av_frame_alloc();
    pOutFrame->sample_rate    = m_nOutSmplRate;
    pOutFrame->format         = m_eOutSmplFmt;
    pOutFrame->nb_samples     = (int)nCvtBufSamples;
    pOutFrame->ch_layout      = m_outChnlLayout;
    int get_buf_res           = av_frame_get_buffer(pOutFrame, 0); // 分配缓冲区
    if (get_buf_res < 0)
    {
        Util::LOGEFMT("<AudioConvert> [ERROR] fail to av_frame_get_buffer(), res=%d\n", get_buf_res);
        av_frame_free(&pOutFrame);
        return -2;
    }
    int convert_res;
    if(pInFrame != nullptr)
    {
        convert_res = swr_convert(m_pSwrCtx,
                                  const_cast<uint8_t **>(pOutFrame->data),
                                  (int) nCvtBufSamples,
                                  const_cast<const uint8_t **>(pInFrame->data),
                                  pInFrame->nb_samples);
    }
    else
    {
        convert_res = swr_convert(m_pSwrCtx,
                                  const_cast<uint8_t **>(pOutFrame->data),
                                  (int) nCvtBufSamples,
                                  nullptr,
                                  0);
    }

    if (convert_res <= 0)
    {
        Util::LOGEFMT("<AudioConvert> no data for swr_convert()\n");
        av_frame_free(&pOutFrame);
        return -1;
    }

    pOutFrame->nb_samples = convert_res;
    if(pInFrame)
    {
        pOutFrame->pts = pInFrame->pts;      // pts等时间戳沿用
        pOutFrame->pkt_dts = pInFrame->pkt_dts;
    }

    *ppOutFrame = pOutFrame;

    return convert_res;
}

void AudioReSampler::Close()
{
    swr_free(&m_pSwrCtx);
}
