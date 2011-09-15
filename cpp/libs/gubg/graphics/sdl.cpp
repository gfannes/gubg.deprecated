#include "graphics/sdl.hpp"
#include "SDL/SDL.h"
#include "boost/thread/mutex.hpp"
#include <exception>

#define L_ENABLE_DEBUG
#include "debug/debug.hpp"

namespace
{
    gubg::SDLPtr sdl;
    boost::mutex mutex;
}

namespace gubg
{
    SDLPtr SDL::initialize()
    {
        boost::mutex::scoped_lock lock(mutex);
        if (!sdl)
            sdl.reset(new SDL);
        return sdl;
    }

    SDL::SDL()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            throw new std::runtime_error("Could not initialize SDL");
        SDL_EnableUNICODE(1);
        DEBUG_PRINT("SDL is initialized");
    }
    SDL::~SDL()
    {
        SDL_Quit();
        DEBUG_PRINT("SDL is finalized");
    }
}
