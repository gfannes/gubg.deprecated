#ifndef HEADER_gubg_log_HR_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_HR_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include <sstream>
#include <vector>
#include <array>
#include <tuple>

namespace gubg { namespace log {

    std::string hr(const double &d)
    {
        return std::to_string(d);
    }
    template <typename T>
        std::string hr(const T &t)
        {
            std::ostringstream oss;
            oss << t;
            return oss.str();
        }
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

    namespace details { 
        template <size_t IX>
            struct StreamTupleValue
            {
                template <typename Tuple>
                    static void stream(std::ostream &os, const Tuple &t)
                    {
                        StreamTupleValue<IX-1>::stream(os, t);
                        os << hr(std::get<IX-1>(t));
                        constexpr size_t tuple_size =  std::tuple_size<Tuple>::value;
                        if (IX == tuple_size)
                            os << "}";
                        else
                            os << ", ";
                    }
            };
        template <>
            struct StreamTupleValue<0>
            {
                template <typename Tuple>
                    static void stream(std::ostream &os, const Tuple &t)
                    {
                        os << "{";
                    }
            };
    } 
    template <typename... Ts>
        std::string hr(const std::tuple<Ts...> &t)
        {
            typedef std::tuple<Ts...> Tuple;
            std::ostringstream oss;
            constexpr size_t tuple_size =  std::tuple_size<Tuple>::value;
            details::StreamTupleValue<tuple_size>::stream(oss, t);
            return oss.str();
        }

} }

#endif
