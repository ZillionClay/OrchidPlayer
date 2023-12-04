//
// Created by 11312 on 2023/11/25.
//

#include "OpenALContext.hpp"

static std::mutex m_Mutex;
std::unique_ptr<OpenALContext> OpenALContext::m_Instance = nullptr;

OpenALContext::OpenALContext(std::string_view _deviceName)
{
    m_Device = alcOpenDevice(_deviceName.data());
//    std::string deviceName = UTF8_To_string(alcGetString(m_Device,ALC_ALL_DEVICES_SPECIFIER));
//    Util::LOGDFMT("<OpenALContext> Output Device: %s\n", deviceName.data());
    m_Context = alcCreateContext(m_Device, nullptr);
    alcMakeContextCurrent(m_Context);
}

OpenALContext::OpenALContext()
{
    m_Device = alcOpenDevice(nullptr); // open defeault m_Device
    m_Context = alcCreateContext(m_Device, nullptr);
    alcMakeContextCurrent(m_Context);
}

OpenALContext::~OpenALContext()
{
    alcMakeContextCurrent(nullptr);
}

int OpenALContext::ListDevice(std::string **pOutStrLst)
{
    const char* lst = alcGetString(nullptr,ALC_ALL_DEVICES_SPECIFIER);
    const char* ptr = lst;
    int numDevice = 0;
    for(;;++ptr)
    {
        if(*ptr == '\0')
        {
            ++numDevice;
            if(*(ptr+1) == '\0') break;
        }
    }
    std::string *outStrLst = new std::string[numDevice]();

    ptr = lst;
    int i = 0;
    for(;; ++ptr)
    {
        if(*ptr != '\0')
            outStrLst[i] += *ptr;
        else
        {
            ++i;
            if(*(ptr+1) == '\0') break;
        }
    }

    *pOutStrLst = outStrLst;

    return numDevice;
}

void OpenALContext::Init()
{
    m_Instance.reset(new OpenALContext);
}

void OpenALContext::QUIT()
{
    m_Instance.reset(nullptr);
}

void OpenALContext::Init(std::string_view device)
{
    m_Instance.reset(new OpenALContext(device));
}

void OpenALContext::ReopenDevice(std::string_view deviceName) noexcept(false)
{

    std::lock_guard<std::mutex> lock(m_Mutex);
    auto ctx = alcGetCurrentContext();
    auto device = alcGetContextsDevice(ctx);

    if(nullptr == device)
    {
        throw std::runtime_error("Context haven't device");
    }

    if(alcIsExtensionPresent(device, "ALC_SOFT_reopen_device"))
    {
        ALCboolean (ALC_APIENTRY*alcReopenDeviceSOFT)(ALCdevice *device, const ALCchar *name, const ALCint *attribs);
        alcReopenDeviceSOFT = reinterpret_cast<decltype(alcReopenDeviceSOFT)>(alcGetProcAddress(device, "alcReopenDeviceSOFT"));
        if(alcReopenDeviceSOFT(device, deviceName.data(), NULL))
        {
            std::string deviceName = UTF8_To_string(alcGetString(m_Device,ALC_ALL_DEVICES_SPECIFIER));
            Util::LOGDFMT("<OpenALContext> Output Device: %s\n", deviceName.data());
            return;
        }
        throw std::runtime_error("Cannot open target device");
    }
    throw std::runtime_error("The OpenAL extension ALC_SOFT_reopen_device is not existing.");
}
