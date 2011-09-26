#ifndef gubg_Platform_hpp
#define gubg_Platform_hpp

#include <string>
#include <sstream>

#ifdef __linux
#define GUBG_LINUX
#endif
#ifdef _WIN32
#define GUBG_WIN32
#endif

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
}

#endif
