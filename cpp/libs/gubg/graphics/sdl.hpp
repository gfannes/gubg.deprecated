#ifndef HEADER_gubg_graphics_sdl_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graphics_sdl_hpp_ALREADY_INCLUDED

#include <memory>

namespace gubg
{
    struct SDL;
    typedef std::shared_ptr<SDL> SDLPtr;
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
