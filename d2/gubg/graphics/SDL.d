module gubg.graphics.SDL;

import std.stdio;
import derelict.sdl.sdl;

scope class SDL
{
    this ()
    {
        version (D_Version2)
        {
            writeln("Bfore loading derelict");
        writefln("SDL_Init = %s", SDL_Init);
        }
        DerelictSDL.load();
        writeln("DerelictSDL is loaded");
        writefln("SDL_Init = %s, %d", SDL_Init, SDL_INIT_VIDEO);
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            writeln("Could not initialize");
            return;
        }
        initialized_ = true;
    }
    ~this ()
    {
        if (initialized_)
        {
            writeln("Calling quit");
            SDL_Quit();
        }
    }

    private:
    bool initialized_;
}

version (UnitTest)
{
    void main()
    {
        scope sdl = new SDL;
    }
}
