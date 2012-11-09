#ifndef HEADER_gubg_string_algo_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_algo_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include <sstream>

namespace gubg
{
    namespace string_algo
    {
        template <typename String>
            size_t nrLeading(const char ch, const String &str)
            {
                for (auto it = str.begin(); it != str.end(); ++it)
                    if (ch != *it)
                        return it - str.begin();
                return str.size();
            }
        template <typename String>
            size_t nrTrailing(const char ch, const String &str)
            {
                for (auto it = str.rbegin(); it != str.rend(); ++it)
                    if (ch != *it)
                        return it - str.rbegin();
                //If all characters are == ch, we call them "leading", making sure you can't count them double
                return 0;
            }

        template <typename Container, typename Delim>
            std::string join(const Container &container, const Delim &delim)
            {
                std::ostringstream oss;
                OnlyOnce skipDelim;
                for (auto v: container)
                {
                    if (!skipDelim())
                        oss << delim;
                    oss << v;
                }
                return oss.str();
            }

        namespace priv
        {
            inline size_t size(const char &){return 1;}
            inline size_t size(const std::string &str){return str.size();}
        }
        template <template <typename T, typename Allocator> class Container, typename Delim, typename Allocator = std::allocator<std::string>>
            Container<std::string, Allocator> split(const std::string &str, const Delim &delim)
            {
                Container<std::string, Allocator> parts;
                size_t pos = 0, ix;
                while (std::string::npos != (ix = str.find(delim, pos)))
                {
                    parts.push_back(str.substr(pos, ix-pos));
                    pos = ix+priv::size(delim);
                }
                parts.push_back(str.substr(pos));
                return parts;
            }
    }
}

#endif
