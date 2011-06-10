#ifndef gubg_Platform_hpp
#define gubg_Platform_hpp

#ifdef __linux
#define GUBG_LINUX
#include <stdlib.h>
#include <unistd.h>
#endif
#ifdef _WIN32
#define GUBG_WIN32
#endif

namespace gubg
{
    bool spawn(const std::string &command)
    {
#ifdef GUBG_LINUX
        switch (fork())
        {
            case -1:
                //Something went wrong
                return false;
                break;
            case 0:
                //Child process
                execl("/bin/sh", "sh", "-c", command.c_str(), (char *)0);
                break;
            default:
                //Parent process, continue without waiting
                return true;
                break;
        }
#endif
        return false;
    }
    template <typename FirstOption, typename ...OptionsRest>
    bool spawn(const std::string &command, FirstOption firstOption, OptionsRest... optionsRest)
    {
        std::ostringstream oss;
        oss << command << " " << firstOption;
        return spawn(oss.str(), optionsRest...);
    }
}

#endif
