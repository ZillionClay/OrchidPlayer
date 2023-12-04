//
// Created by 11312 on 2023/11/25.
//

#include "OpenALBuffers.hpp"

OpenALBuffer::OpenALBuffer()
{
    alGenBuffers(1, &m_bid);
}

OpenALBuffer::OpenALBuffer(uint32_t bid)
{
    m_bid = bid;
}

void OpenALBuffer::DeleteBuf()
{
    alDeleteBuffers(1, &m_bid);
}

void OpenALBuffer::SetData(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
    alBufferData(m_bid, format, data, size, freq);
}

OpenALBuffers::OpenALBuffers(uint32_t n)
{
    m_bidArr.resize(n);
    alGenBuffers((int)n, m_bidArr.data());
}

void OpenALBuffers::DeleteBufs()
{
    alDeleteBuffers((int)m_bidArr.size(), m_bidArr.data());
}