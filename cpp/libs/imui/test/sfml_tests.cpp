#include "imui/sfml.hpp"

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace { 
    class App: public imui::App
    {
        public:
            App(): imui::App(100, 200, __FILE__) {}

            void render(const imui::Region &region) override
            {
                auto r = region.sub(50, 150, 100, 100);
                if (r.isMouseInside())
                {
                    S();L(region.mouse().x << " " << region.mouse().y);
                }
            }

        private:
    };
} 

int main()
{
    App app;
    while (app())
    {
    }
    return 0;
}
#include "gubg/log/end.hpp"
