#ifndef HEADER_gubg_log_HR_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_HR_hpp_ALREADY_INCLUDED

#include <sstream>
#include <vector>

namespace gubg
{
    namespace log
    {
        template <typename T>
            std::string hr(const std::vector<T> &vec)
            {
                std::ostringstream oss;
                oss << '{';
                for (auto v: vec)
                    oss << v << ", ";
                oss << '}';
                return oss.str();
            }
    }
}

#endif
