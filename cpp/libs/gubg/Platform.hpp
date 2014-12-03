#ifndef HEADER_gubg_Platform_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Platform_hpp_ALREADY_INCLUDED

#include <string>
#include <sstream>

//For now, gubg is only supported on little endian platforms
#define GUBG_LITTLE_ENDIAN

//Compiler info
#ifdef __GNUC__
#define GUBG_CC_GCC
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif
#ifdef _MSC_VER
#define GUBG_CC_MSC
#endif

//OS API info
#ifdef __linux
#define GUBG_API_LINUX
#define GUBG_API_POSIX
#endif//__linux
#ifdef _WIN32
#define GUBG_API_WIN32
#ifdef __MINGW32__
#define GUBG_API_MINGW
#define GUBG_API_POSIX
#endif//__MINGW32__
#endif//_WIN32

namespace gubg
{
    bool spawn(const std::string &command);

    template <typename FirstOption, typename ...OptionsRest>
    bool spawn(const std::string &command, FirstOption firstOption, OptionsRest... optionsRest)
    {
        std::ostringstream oss;
        oss << command << " " << firstOption;
        return spawn(oss.str(), optionsRest...);
    }

    //Basic command-line escaping
    enum class Quotes {Add};
    std::string escapeForCLI(const std::string &str, Quotes quotes);

    typedef unsigned long ProcessId;
    ProcessId processId();
    typedef unsigned long ThreadId;
    unsigned long threadId();

    double durationSinceEpoch_sec();
}

#endif
