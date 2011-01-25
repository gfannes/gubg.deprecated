module gubg.graphics.SDL;

import std.stdio;
import derelict.sdl.sdl;

//Reference-count SDL initializer
class SDL
{
    this()
    {
        if (0 == refCount__)
        {
            DerelictSDL.load();
            if (SDL_Init(SDL_INIT_VIDEO) < 0)
                throw new Exception("Could not initialize SDL");
            SDL_EnableUNICODE(1);
        }
        ++refCount__;
    }
    ~this()
    {
        finalize();
    }
    static ~this()
    {
        finalize();
    }

    private:
    static void finalize()
    {
        if (1 == refCount__)
            SDL_Quit();
        //The destructor might run before the static destructor
        if (refCount__ > 0)
            --refCount__;
    }
    static uint refCount__ = 0;
}

version (UnitTest)
{
    void main()
    {
        auto sdl = new SDL;
    }
}
