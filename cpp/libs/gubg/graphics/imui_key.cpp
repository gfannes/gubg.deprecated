#include "graphics/imui_key.hpp"

std::ostream &operator<<(std::ostream &os, gubg::Key key)
{
    return os << (int)key;
}
bool operator<=(gubg::Key lhs, gubg::Key rhs)
{
    return (int)lhs <= (int)rhs;
}
namespace gubg
{
    Key fromSDL(int sdlKey){return (Key)sdlKey;}
    bool convertToDigit(unsigned char &digit, Key key)
    {
        if (Key::Dec0 <= key && key <= Key::Dec9)
        {
            digit = (unsigned char)key - (unsigned char)Key::Dec0;
            return true;
        }
        if (Key::DecKP0 <= key && key <= Key::DecKP9)
        {
            digit = (unsigned char)key - (unsigned char)Key::DecKP0;
            return true;
        }
        return false;
    }
    char convertToChar(Key key)
    {
        unsigned char digit;
        if (convertToDigit(digit, key))
            return '0'+digit;
        if (Key::a <= key && key <= Key::z)
            return 'a'+(char)key-(char)Key::a;
        if (Key::A <= key && key <= Key::Z)
            return 'A'+(char)key-(char)Key::A;
        switch (key)
        {
            case Key::Return: return '\n';
            case Key::Colon: return ':';
            case Key::Semicolon: return ';';
            case Key::Comma: return ',';
            case Key::Space: return ' ';
            case Key::Underscore: return '_';
            case Key::Period: return '.';
            case Key::Slash: return '/';
            case Key::Backslash: return '\\';
            case Key::LeftParenthesis: return '(';
            case Key::RightParenthesis: return ')';
            case Key::LeftBracket: return '[';
            case Key::RightBracket: return ']';
            default: return '.';
        }
        assert(false);
        return '.';
    }
}
