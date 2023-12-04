#include <QApplication>
#include "widget.hpp"
#include <iostream>
#include <string>
#include <thread>
#include "util.hpp"
#include "OpenALContext.hpp"
#include "AudioPlayer.hpp"
#include <sstream>

static bool CmdBufRead(std::mutex& cmdBufLock, std::queue<std::string>& cmdBuf, std::string& out)
{
    std::lock_guard<std::mutex> lock(cmdBufLock);
    if(cmdBuf.empty())
    {
        return false;
    }
    out = cmdBuf.front();
    cmdBuf.pop();
    return true;
}

void execute()
{
    std::string *lst = nullptr;
    int numDevice = OpenALContext::ListDevice(&lst);
    Util::LOGDFMT("<Main>Listing %d available device.\n", numDevice);
    Util::OUTD("________________________\n");
    for (int i = 0; i < numDevice; ++i)
        std::cout << i << ": " << UTF8_To_string(lst[i]) << std::endl;
    Util::OUTD("________________________\n");

    AudioPlayer player;

    OpenALContext& alctx = OpenALContext::GetInstance();

    std::mutex cmdBufLock;
    std::queue<std::string> cmdBuf;

    std::thread ([&cmdBufLock, &cmdBuf]()->void
                 {
                     for(;;)
                     {
                         std::string str;
                         std::getline(std::cin, str);
                         {
                             std::lock_guard<std::mutex> lock(cmdBufLock);
                             cmdBuf.push(str);
                         }
                     }
                 }).detach();

    std::string cmdline;

    for(;;)
    {
        bool isBufEmpty = !CmdBufRead(cmdBufLock, cmdBuf, cmdline);

        if(isBufEmpty)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        std::stringstream scmd(cmdline);

        std::string param;
        scmd >> param;

        if(param == "open")
        {
            scmd >> param;
            try
            {
                if (player.Open(param))
                {
                    uint64_t milliseconds = static_cast<uint64_t>(player.GetDuration() * 1000);
                    Util::LOGDFMT("<Main> Duration of the song: %dm %ds %dms\n",
                                  milliseconds / 60000,
                                  (milliseconds % 60000) / 1000,
                                  (milliseconds % 60000) % 1000);
                }
            }
            catch (...)
            {
                Util::LOGEFMT("<Main> %s", "Cannot open url\n");
            }
            player.Start();
        }
        else if(param == "exit")
        {
            player.Close();
            break;
        }
        else if(param == "close")
        {
            player.Close();
        }
        else if(param == "reopen")
        {
            int index;
            scmd >> index;
            if(index < 0 || index >= numDevice) continue;
            try
            {
                alctx.ReopenDevice(lst[index]);
            }
            catch (std::runtime_error& re)
            {
                Util::LOGEFMT("<Main> %s\n", re.what());
            }
        }
        else if(param == "stop")
        {
            player.Stop();
        }
        else if(param == "start")
        {
            player.Start();
        }
        else if(param == "tell")
        {
            uint64_t milliseconds = static_cast<uint64_t>(player.Tell()*1000);
            Util::OUTD(milliseconds/60000, "m ",
                        (milliseconds%60000)/1000, "s ",
                        (milliseconds%60000)%1000, "ms :",
                        (int)(player.Tell()*100/player.GetDuration()), "%\n");
        }
        else if(param == "jump")
        {
            double second;
            scmd >> second;
            player.Locate(second);
        }

    }

    delete[] lst;
}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    OpenALContext::Init(); //初始化OpenAL上下文

    Widget w;
    w.show();
    return QApplication::exec();
}
