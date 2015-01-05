#ifndef HEADER_imui_Stream_hpp_ALREADY_INCLUDED
#define HEADER_imui_Stream_hpp_ALREADY_INCLUDED

#include <ostream>

namespace sf { 

    template <typename T>
        std::ostream &operator<<(std::ostream &os, const Vector2<T> &v)
        {
            os << "[" << v.x << ", " << v.y << "]";
            return os;
        }

} 

#endif
