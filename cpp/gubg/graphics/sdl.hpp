#ifndef gubg_graphics_sdl_hpp
#define gubg_graphics_sdl_hpp

#include "boost/shared_ptr.hpp"

namespace gubg
{
    struct SDL;
    typedef boost::shared_ptr<SDL> SDLPtr;
    struct SDL
    {
        public:
            static SDLPtr initialize();
            ~SDL();

        private:
            SDL();
    };
}

#endif
