#ifndef HEADER_gubg_string_algo_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_algo_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include <sstream>
#include <cctype>

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
        template <template <typename T, typename Allocator> class Container, typename Allocator = std::allocator<std::string>>
            Container<std::string, Allocator> splitLines(const std::string &str)
            {
                Container<std::string, Allocator> parts;
		const auto s = str.size();
                size_t pos = 0, ix;
                while (std::string::npos != (ix = str.find_first_of("\n\r", pos, 2)))
                {
			if (str[ix] == '\n')
			{
				//Single line-feed found
				parts.push_back(str.substr(pos, ix-pos));
				pos = ix+1;
			}
			else if (str[ix] == '\r' && ix+1 < s && str[ix+1] == '\n')
			{
				//Carriage-return/Line-feed is found
				parts.push_back(str.substr(pos, ix-pos));
				pos = ix+2;
			}
			else
				pos = ix+1;
                }
                parts.push_back(str.substr(pos));
                return parts;
            }

	template <typename String>
		void upcase(String &str)
		{
			for (auto &ch: str)
				ch = std::toupper(ch);
		}
    }
}

#endif
