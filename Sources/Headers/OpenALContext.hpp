//
// Created by 11312 on 2023/11/25.
//

#ifndef ORCHIDPLAYER_OPENALCONTEXT_HPP
#define ORCHIDPLAYER_OPENALCONTEXT_HPP

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "OpenAL/alext.h"
#include "OpenAL/efx.h"
#include "OpenAL/efx-creative.h"
#include "OpenAL/efx-presets.h"
#include "util.hpp"
#include <string>
#include <memory>
#include <cassert>
#include <mutex>

class OpenALContext final
{
public:

    ALCdevice* m_Device = nullptr;
    ALCcontext* m_Context = nullptr;

    static void Init();

    static void Init(std::string_view device);

    static void QUIT();

    static int ListDevice(std::string** pOutStrLst);

    inline static OpenALContext &GetInstance()
    {
        assert(m_Instance);
        return *m_Instance;
    }

    void ReopenDevice(std::string_view deviceName) noexcept(false);

    OpenALContext(const OpenALContext& other) = delete;
    OpenALContext& operator=(const OpenALContext& other) = delete;
    ~OpenALContext();

private:

    std::mutex m_Mutex;
    static std::unique_ptr<OpenALContext> m_Instance;

    OpenALContext();

    explicit OpenALContext(std::string_view deviceName);

};


#endif //ORCHIDPLAYER_OPENALCONTEXT_HPP
