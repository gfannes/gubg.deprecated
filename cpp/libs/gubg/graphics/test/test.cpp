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
    //Color and Style tests
    {
        cout << Color::red << endl;
        cout << Color(0.1, 0.2, 0.3) << endl;
        cout << Color({0.1, 0.2, 0.3}) << endl;

        cout << endl << "Symmetric shades between -1 and 1" << endl;
        cout << Color::shade<-1, 1>(-0.1) << endl;
        cout << Color::shade<-1, 1>(0) << " " << Color::black << endl;
        cout << Color::shade<-1, 1>(0.1) << endl;

        cout << endl << "Asymmetric shades between 0 and 1" << endl;
        cout << Color::shade<0, 1>(0.4) << endl;
        cout << Color::shade<0, 1>(0.5) << " " << Color::olive << endl;
        cout << Color::shade<0, 1>(0.6) << endl;

        cout << endl;
        cout << Style().fill(Color::red).stroke(Color::green).width(1.5) << endl;
    }

    //Multiple initialization of SDL
    auto sdl1 = SDL::initialize();
    auto sdl2 = SDL::initialize();

    //Creation of the SDL canvas
    SDLCanvas canvas(200, 100);

    //Draw some primitives on the canvas
    {
        //Functor that will be called by Sequence
        struct DrawColor: Sequence<double>::EachBlock
        {
            DrawColor(SDLCanvas &c):
                canvas(c){}
            SDLCanvas &canvas;
            bool yield(double v)
            {
                ICanvas::DrawScope sc(canvas);
                canvas.clear(Color::shade<0, 1>(v));
                canvas.drawCircle(Point<>(50+v*100, 50), 40, Style().stroke(Color::shade<-1, 1>(-1.0+v)).width(3));
                canvas.drawRectangle(TwoPoint<>(25+v*100, 25, 75+v*100, 75), Style().stroke(Color::shade<-1, 1>(v)).width(3));
                nanosleep(0, 5000000);
                return true;
            }
        } dc(canvas);
        for (int i = 0; i < 3; ++i)
        {
            //From red to green
            Sequence<double>(0, 1, 0.01).each(dc);
            //From green to red
            Sequence<double>(1, 0, -0.01).each(dc);
        }
    }

    //Demonstrate the creation of and handling of button, IMUI-style
    Widgets widgets;
    Timer timer(ResetType::NoAuto);
    while (timer.difference() < 10)
    {
        if (canvas.getIMUI()->escapeIsPressed())
            break;
        ICanvas::DrawScope sc(canvas);

        //Create 4 placeholders for widgets. We will get _the same_ place holders each time we
        //loop, but they will all 4 be different since widgets.get() checks who is calling it
        {
            WidgetProxy &b1 = widgets.get(GUBG_HERE());
            WidgetProxy &b2 = widgets.get(GUBG_HERE());
            WidgetProxy &b3 = widgets.get(GUBG_HERE());
            WidgetProxy &b4 = widgets.get(GUBG_HERE());
            switch (b1.process())
            {
                case WidgetState::Empty:
                    b1.set(auto_ptr<IWidget>(new Button(TwoPoint<>(0, 0, 100, 25), "Named1", Alignment::Left, canvas)));
                    break;
                case WidgetState::Activated:
                    cout << "Named1 was pressed" << endl;
                    break;
            }
            switch (b2.process())
            {
                case WidgetState::Empty:
                    b2.set(auto_ptr<IWidget>(new Button(TwoPoint<>(0, 25, 100, 50), "Named2", Alignment::Left, canvas)));
                    break;
                case WidgetState::Activated:
                    cout << "Named2 was pressed" << endl;
                    break;
            }
            switch (b3.process())
            {
                case WidgetState::Empty:
                    b3.set(auto_ptr<IWidget>(new Button(TwoPoint<>(0, 50, 100, 75), "Named3", Alignment::Left, canvas)));
                    break;
                case WidgetState::Activated:
                    cout << "Named3 was pressed" << endl;
                    break;
            }
            switch (b4.process())
            {
                case WidgetState::Empty:
                    b4.set(auto_ptr<IWidget>(new Button(TwoPoint<>(0, 75, 100, 100), "Named4", Alignment::Left, canvas)));
                    break;
                case WidgetState::Activated:
                    cout << "Named4 was pressed" << endl;
                    break;
            }
        }

        //We use a for-loop to create 4 similar buttons, but now the source code location of the caller of widgets.get()
        //is each time _the same_, so we need to insert extra differentiating info to enable widgets.get() to discriminate between them
        {
            for (int i = 0; i < 4; ++i)
            {
                WidgetProxy &b = widgets.get(GUBG_HERE(), i);
                std::ostringstream oss;
                oss << "Unnamed" << i;
                switch (b.process())
                {
                    case WidgetState::Empty:
                        b.set(auto_ptr<IWidget>(new Button(TwoPoint<>(100, i*25, 200, (i+1)*25), oss.str(), Alignment::Left, canvas)));
                        break;
                    case WidgetState::Activated:
                        cout << oss.str() << " was pressed" << endl;
                        break;
                }
            }
        }

        //Don't burn the CPU
        nanosleep(0, 10000000);
    }

    return 0;
}
