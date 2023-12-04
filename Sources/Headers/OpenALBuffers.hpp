//
// Created by 11312 on 2023/11/25.
//

#ifndef ORCHIDPLAYER_OPENALBUFFERS_HPP
#define ORCHIDPLAYER_OPENALBUFFERS_HPP

#include <cstdint>
#include "OpenAL\al.h"
#include "OpenAL\alc.h"
#include <vector>

class OpenALBuffers;

class OpenALBuffer
{
private:
    uint32_t m_bid;
public:

    OpenALBuffer(); // 自动申请一块缓冲区
    explicit OpenALBuffer(uint32_t bid); // 用已有的bid初始化
    ~OpenALBuffer() = default;

    void SetData(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);

    void DeleteBuf(); // 删除时要确保拥有实际缓冲区的所有权

    inline const uint32_t& GetID() const
    {
        return m_bid;
    }

    inline uint32_t& GetID()
    {
        return m_bid;
    }
};


class OpenALBuffers
{
private:
    std::vector<uint32_t> m_bidArr;
public:

    OpenALBuffers()= default;
    explicit OpenALBuffers(uint32_t n);
    ~OpenALBuffers() = default;

    void DeleteBufs();

    inline OpenALBuffer operator[](int i)
    {
        return OpenALBuffer(m_bidArr[i]);
    }

    inline OpenALBuffer at(int i)
    {
        return OpenALBuffer(m_bidArr.at(i));
    }

    inline const std::vector<uint32_t>& GetIDArr() const
    {
        return m_bidArr;
    }

    inline std::vector<uint32_t>& GetIDArr()
    {
        return m_bidArr;
    }

};


#endif //ORCHIDPLAYER_OPENALBUFFERS_HPP
