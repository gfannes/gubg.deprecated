#include "gubg/graphics/sdl.hpp"
#include "SDL/SDL.h"
#include "boost/thread/mutex.hpp"
#include <exception>

namespace
{
    gubg::SDLPtr sdl;
    boost::mutex mutex;
}

#define GUBG_MODULE "SDL"
#include "gubg/log/begin.hpp"
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
        S();
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            throw new std::runtime_error("Could not initialize SDL");
        SDL_EnableUNICODE(1);
        L("SDL is initialized");
    }
    SDL::~SDL()
    {
        S();
        SDL_Quit();
        L("SDL is finalized");
    }
}
#include "gubg/log/end.hpp"
