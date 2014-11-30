#ifndef HEADER_gubg_log_HR_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_HR_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include <sstream>
#include <vector>
#include <array>

namespace gubg { namespace log {

    template <typename T>
        std::string hr(const std::vector<T> &vec)
        {
            std::ostringstream oss;
            oss << '{';
            OnlyOnce skipComma;
            for (auto v: vec)
                oss << (skipComma() ? "" : ", ") << v;
            oss << '}';
            return oss.str();
        }

    template <typename T, size_t N>
        std::string hr(const std::array<T, N> &ary)
        {
            std::ostringstream oss;
            oss << '{';
            OnlyOnce skipComma;
            for (auto v: ary)
                oss << (skipComma() ? "" : ", ") << v;
            oss << '}';
            return oss.str();
        }

} }

#endif
