#include "graphics/style.hpp"
#include "graphics/sdl.hpp"
#include "graphics/canvas.hpp"
#include "graphics/sdl_canvas.hpp"
#include <iostream>
#include <array>
using namespace gubg;
using namespace std;

int main(int argc, char *argv[])
{
    cout << Color::red << endl;
    cout << Color(0.1, 0.2, 0.3) << endl;
    cout << Color({0.1, 0.2, 0.3}) << endl;
    cout << Color(0.1) << endl;
    cout << Color(-0.1) << endl;

    cout << Style().fill(Color::red).stroke(Color::green).width(1.5) << endl;

    auto sdl1 = SDL::initialize();
    auto sdl2 = SDL::initialize();

    SDLCanvas canvas(100, 100);
    ICanvas::DrawScope sc(canvas);
    
    for (int i = 0; i < 1000000; ++i)
        ;
    return 0;
}
