#ifndef HEADER_rinle_view_Util_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Util_hpp_ALREADY_INCLUDED

#include <nana/deploy.hpp>
#include <algorithm>

namespace rinle
{
    namespace view
    {
        template <typename String>
            void convert(nana::string &dst, const String &src)
            {
                dst.resize(src.size());
                std::copy(src.begin(), src.end(), dst.begin());
            }
    }
}

#endif
