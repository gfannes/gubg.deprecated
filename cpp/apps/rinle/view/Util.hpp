#ifndef HEADER_rinle_view_Util_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Util_hpp_ALREADY_INCLUDED

#include <nana/deploy.hpp>
#include <algorithm>
#include <sstream>

namespace rinle
{
    namespace view
    {
		inline void convert(nana::string &dst, const std::string &src)
		{
			dst.resize(src.size());
			std::copy(src.begin(), src.end(), dst.begin());
		}
		inline void convert(nana::string &dst, int nr)
		{
			std::ostringstream oss; oss << nr;
			convert(dst, oss.str());
		}
	}
}

#endif
