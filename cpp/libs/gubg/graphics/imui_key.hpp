#ifndef gubg_graphics_imui_key_hpp
#define gubg_graphics_imui_key_hpp

#include "SDL/SDL_keysym.h"
#include <cassert>
#include <ostream>

namespace gubg
{
    enum class Key
    {
        Nil = 0,

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

        a = SDLK_a,
        b = SDLK_b,
        c = SDLK_c,
        d = SDLK_d,
        e = SDLK_e,
        f = SDLK_f,
        g = SDLK_g,
        h = SDLK_h,
        i = SDLK_i,
        j = SDLK_j,
        k = SDLK_k,
        l = SDLK_l,
        m = SDLK_m,
        n = SDLK_n,
        o = SDLK_o,
        p = SDLK_p,
        q = SDLK_q,
        r = SDLK_r,
        s = SDLK_s,
        t = SDLK_t,
        u = SDLK_u,
        v = SDLK_v,
        w = SDLK_w,
        x = SDLK_x,
        y = SDLK_y,
        z = SDLK_z,

        A = SDLK_a - SDLK_a + 'A',
        B = SDLK_b - SDLK_a + 'A',
        C = SDLK_c - SDLK_a + 'A',
        D = SDLK_d - SDLK_a + 'A',
        E = SDLK_e - SDLK_a + 'A',
        F = SDLK_f - SDLK_a + 'A',
        G = SDLK_g - SDLK_a + 'A',
        H = SDLK_h - SDLK_a + 'A',
        I = SDLK_i - SDLK_a + 'A',
        J = SDLK_j - SDLK_a + 'A',
        K = SDLK_k - SDLK_a + 'A',
        L = SDLK_l - SDLK_a + 'A',
        M = SDLK_m - SDLK_a + 'A',
        N = SDLK_n - SDLK_a + 'A',
        O = SDLK_o - SDLK_a + 'A',
        P = SDLK_p - SDLK_a + 'A',
        Q = SDLK_q - SDLK_a + 'A',
        R = SDLK_r - SDLK_a + 'A',
        S = SDLK_s - SDLK_a + 'A',
        T = SDLK_t - SDLK_a + 'A',
        U = SDLK_u - SDLK_a + 'A',
        V = SDLK_v - SDLK_a + 'A',
        W = SDLK_w - SDLK_a + 'A',
        X = SDLK_x - SDLK_a + 'A',
        Y = SDLK_y - SDLK_a + 'A',
        Z = SDLK_z - SDLK_a + 'A',

        Return = SDLK_RETURN,
        Colon = SDLK_COLON,
        Semicolon = SDLK_SEMICOLON,
        Comma = SDLK_COMMA,
        Space = SDLK_SPACE,
        Underscore = SDLK_UNDERSCORE,
        Period = SDLK_PERIOD,
        Slash = SDLK_SLASH,
        Backslash = SDLK_BACKSLASH,
        Delete = SDLK_DELETE,
        Backspace = SDLK_BACKSPACE,

        Up = SDLK_UP,
        Down = SDLK_DOWN,
        Right = SDLK_RIGHT,
        Left = SDLK_LEFT,
        PageUp = SDLK_PAGEUP,
        PageDown = SDLK_PAGEDOWN,

        LeftParenthesis = SDLK_LEFTPAREN,
        RightParenthesis = SDLK_RIGHTPAREN,
        LeftBracket = SDLK_LEFTBRACKET,
        RightBracket = SDLK_RIGHTBRACKET,

        Escape = SDLK_ESCAPE,
    };
    //Key is basically the same order as SDL uses...
    Key fromSDL(int sdlKey);
    bool convertToDigit(unsigned char &digit, Key key);
    char convertToChar(Key key);
}
std::ostream &operator<<(std::ostream &os, gubg::Key);
bool operator<=(gubg::Key, gubg::Key);

#endif
