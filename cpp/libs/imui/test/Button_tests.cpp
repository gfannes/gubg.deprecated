#include "imui/App.hpp"
#include "imui/Button.hpp"
#include <iostream>

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
class App: public imui::App
{
    public:
        App(): imui::App(640, 480, __FILE__) {}
    private:
        void render(const imui::Region &r) override
        {
            using namespace imui;
            Button b(r, IMUI_ID()); b.setPosition(Vector2{100, 100}).setSize(Vector2{30, 30});
            switch (b())
            {
                case Event::Clicked:
                    {
                        S();L("Button " << b.id() << " was clicked");
                        quitASAP();
                    }
                    break;
            }
            for (int ix = 0; ix < 5; ++ix)
            {
                Button b(r, IMUI_ID_IX(ix)); b.setPosition(Vector2{ix*50+50, 200}).setSize(Vector2{30, 30});
                switch (b())
                {
                    case Event::Clicked:
                        {
                            S();L("Button " << b.id() << " was clicked");
                        }
                        break;
                }
            }
        }
};

int main()
{
    App app;
    while (app())
    {
    }
    return 0;
}
#include "gubg/log/end.hpp"
