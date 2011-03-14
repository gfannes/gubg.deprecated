#include "graphics/style.hpp"
#include "graphics/sdl.hpp"
#include "graphics/canvas.hpp"
#include "graphics/sdl_canvas.hpp"
#include "graphics/imui.hpp"
#include "graphics/imui_button.hpp"
#include "sleep.hpp"
#include "timer.hpp"
#include "sequence.hpp"
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

    struct DrawColor: Sequence<double>::EachBlock
    {
        DrawColor(SDLCanvas &c):
            canvas(c){}
        SDLCanvas &canvas;
        bool yield(double v)
        {
            ICanvas::DrawScope sc(canvas);
            canvas.clear(Color(v));
            canvas.drawCircle(Point<>(50, 50), 40, Style().stroke(Color::red).width(3));
            canvas.drawRectangle(TwoPoint<>(25, 25, 75, 75), Style().stroke(Color::green).width(3));
            nanosleep(0, 5000000);
            return true;
        }
    } dc(canvas);
    for (int i = 0; i < 3; ++i)
    {
        //From red to green
        Sequence<double>(-1, 1, 0.01).each(dc);
        //From green to red
        Sequence<double>(1, -1, -0.01).each(dc);
    }

    Widgets widgets;
    Timer timer(ResetType::NoAuto);
    while (timer.difference() < 10)
    {
        ICanvas::DrawScope sc(canvas);
        WidgetProxy &b1 = widgets.get();
        switch (b1.process())
        {
            case WidgetState::Empty:
                b1.set(auto_ptr<IWidget>(new Button(TwoPoint<>(0, 0, 100, 25), "Boerke", Alignment::Left, canvas)));
                break;
        }
        nanosleep(0, 10000000);
    }

    return 0;
}
