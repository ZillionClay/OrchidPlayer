//
// Created by 11312 on 2023/11/23.
//

#ifndef ORCHIDPLAYER_UTIL_HPP
#define ORCHIDPLAYER_UTIL_HPP

#include <string_view>
#include <iostream>
#include <string>
#include <format>
#include <windows.h>
#include <mutex>
#include <sstream>
#include <functional>

std::string string_To_UTF8(const std::string & str);
std::string UTF8_To_string(const std::string & str);

namespace Util
{
    static std::mutex logMutex;

    using ControlFlag = std::function<void(std::ostream&, std::stringstream&)>;

    static ControlFlag endl = [](std::ostream& out, std::stringstream& ss)
    {
        ss << "\n";
        out << ss.str();
        ss.str("");
    };

    template<std::ostream& out>
    class OutStream
    {
    private:
        std::stringstream m_Buf;
        explicit OutStream()
        {
            m_Buf.str("");
        }

    public:

        static OutStream& GetInstance()
        {
            static OutStream<out> Instance_;
            return Instance_;
        }

        template<typename T>
        OutStream& operator<<(T&& arg)
        {
            arg(*this);
            m_Buf << arg;
            return *this;
        }
    };


    template<typename ...Args>
    static void OUTD(Args&& ...args)
    {
        std::lock_guard<std::mutex> lock(logMutex);
        int _[] = {(std::cout << std::forward<Args>(args), 0)...};
    }

    template<typename ...Args>
    static void OUTE(Args&& ...args)
    {
        std::lock_guard<std::mutex> lock(logMutex);
        int _[] = {(std::cerr << std::forward<Args>(args), 0)...};
    }

    template<typename ...Args>
    static void LOGDFMT(std::string_view info, Args&& ...args)
    {
        std::lock_guard<std::mutex> lock(logMutex);
        printf("[LOG] ");
        printf(info.data(), std::forward<Args>(args)...);
    }

    template<typename ...Args>
    static void LOGEFMT(std::string_view info, Args&& ...args)
    {
        std::lock_guard<std::mutex> lock(logMutex);
        fprintf(stderr, "[ERR] ");
        fprintf(stderr, info.data(), std::forward<Args>(args)...);
    }
}

#endif //ORCHIDPLAYER_UTIL_HPP
