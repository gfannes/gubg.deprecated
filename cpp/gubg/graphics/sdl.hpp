#ifndef gubg_graphics_sdl_hpp
#define gubg_graphics_sdl_hpp

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
