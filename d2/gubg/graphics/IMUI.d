module gubg.graphics.IMUI;

import gubg.graphics.Canvas;
import derelict.sdl.sdl;

import std.stdio;

//Key is basically the same order as SDL uses...
Key fromSDL(int sdlKey){return cast(Key)sdlKey;}
bool convertToDigit(out ubyte digit, in Key key)
{
    if (Key.Dec0 <= key && key <= Key.Dec9)
    {
        digit = cast(ubyte)(key - Key.Dec0);
        return true;
    }
    if (Key.DecKP0 <= key && key <= Key.DecKP9)
    {
        digit = cast(ubyte)(key - Key.DecKP0);
        return true;
    }
    return false;
}
enum Key
{
    None = 0,

    //Normal numbers
    Dec0 = SDLK_0,
    Dec1 = SDLK_1,
    Dec2 = SDLK_2,
    Dec3 = SDLK_3,
    Dec4 = SDLK_4,
    Dec5 = SDLK_5,
    Dec6 = SDLK_6,
    Dec7 = SDLK_7,
    Dec8 = SDLK_8,
    Dec9 = SDLK_9,
    //Keypad numbers
    DecKP0 = SDLK_KP0,
    DecKP1 = SDLK_KP1,
    DecKP2 = SDLK_KP2,
    DecKP3 = SDLK_KP3,
    DecKP4 = SDLK_KP4,
    DecKP5 = SDLK_KP5,
    DecKP6 = SDLK_KP6,
    DecKP7 = SDLK_KP7,
    DecKP8 = SDLK_KP8,
    DecKP9 = SDLK_KP9,

    //Function keys
    F1 = SDLK_F1,
    F2 = SDLK_F2,
    F3 = SDLK_F3,
    F4 = SDLK_F4,
    F5 = SDLK_F5,
    F6 = SDLK_F6,
    F7 = SDLK_F7,
    F8 = SDLK_F8,
    F9 = SDLK_F9,
    F10 = SDLK_F10,
    F11 = SDLK_F11,
    F12 = SDLK_F12,
    F13 = SDLK_F13,
    F14 = SDLK_F14,
    F15 = SDLK_F15,

    Escape = SDLK_ESCAPE,
}

//Immediate-mode user interface
abstract class IMUI
{
    //Call this fast enough to get a reasonable response time
    //Returns true if some event is waiting (not sure if this maps OK for all input devices)
    //Calling this twice without getting any event should return the same
    bool processInput();

    //This is a non-const method, it will reset the interal lastKey_
    Key getLastKey()
    {
        Key key = lastKey_;
        lastKey_ = Key.None;
        return key;
    }
    //This is a non-const method, it will reset the internal lastKey_ if a digit was found
    bool getDigit(out ubyte digit)
    {
        if (convertToDigit(digit, lastKey_))
        {
            getLastKey();
            return true;
        }
        return false;
    }
    //This is a non-const method, it will reset the internal lastKey_ if ESC was found
    bool escapeIsPressed()
    {
        if (Key.Escape == lastKey_)
        {
            getLastKey();
            return true;
        }
        return false;
    }

    protected:
    Key lastKey_ = Key.None;
}

version (UnitTest)
{
    import core.thread;
    void main()
    {
        auto canvas = new SDLCanvas(640, 480);
        auto imui = new SDLIMUI(canvas);
        int i = 0;
        while (!imui.escapeIsPressed)
        {
            imui.processInput();
            ++i;
            if (i > 100)
                return;
            //Sleep for 10ms
            Thread.sleep(100000);
        }
    }
}
