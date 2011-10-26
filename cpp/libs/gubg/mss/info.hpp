#ifndef gubg_mss_info_h
#define gubg_mss_info_h

#include "gubg/Singleton.hpp"
#include "gubg/Location.hpp"
#include <string>
#include <map>
#include <iostream>

namespace gubg
{
    namespace mss
    {
        //The logging level of a certain ReturnCode
        enum class Level {Unknown, OK, Debug, Info, Warning, Error, Critical, Fatal};

        //Information about a certain ReturnCode: its logging level and a human readable representation
        struct Info
        {
            Info(){}
            Info(Level l, const std::string &t, const std::string &c):
                level(l),
                type(t),
                code(c){}
            Level level;
            std::string type;
            std::string code;
            gubg::Location location;
        };

        //Manages ReturnCode information per ReturnCode type
        template <typename ReturnCode>
        class InfoMgr
        {
            private:
                GUBG_SINGLETON(InfoMgr<ReturnCode>);
            public:
                void set(ReturnCode code, Level level, const std::string &t, const std::string &c)
                {
                    infoPerCode_[code] = Info(level, t, c);
                }
                Info get(ReturnCode code)
                {
                    typename InfoPerCode::iterator it = infoPerCode_.find(code);
                    if (it == infoPerCode_.end())
                        return Info(Level::Error, "?", "?");
                    return it->second;
                }
            private:
                typedef std::map<ReturnCode, Info> InfoPerCode;
                InfoPerCode infoPerCode_;
        };
        template <typename ReturnCode>
            InfoMgr<ReturnCode>::InfoMgr(){}

        //A helper class that can be used to set ReturnCode information into InfoMgr even before main() is running
        template <typename ReturnCode>
            struct InfoSetter
            {
                InfoSetter(ReturnCode code, Level level, const std::string &t, const std::string &c)
                {
                    InfoMgr<ReturnCode>::instance().set(code, level, t, c);
                }
            };

        //A getter in the form of a template function, which has as benefit that we can use automatic argument deduction
        template <typename ReturnCode>
            Info getInfo(ReturnCode code)
            {
                return InfoMgr<ReturnCode>::instance().get(code);
            }
    }
}
std::ostream &operator<<(std::ostream &os, gubg::mss::Level level)
{
    switch (level)
    {
        case gubg::mss::Level::OK:       return os << "<OK>";
        case gubg::mss::Level::Debug:    return os << "<Debug>";
        case gubg::mss::Level::Info:     return os << "<Info>";
        case gubg::mss::Level::Warning:  return os << "<Warning>";
        case gubg::mss::Level::Error:    return os << "<Error>";
        case gubg::mss::Level::Critical: return os << "<Critical>";
        case gubg::mss::Level::Fatal:    return os << "<Fatal>";
        default: os << "<Unknown level>";
    }
    return os;
}

#endif
