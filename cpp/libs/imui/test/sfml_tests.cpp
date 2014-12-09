#include "imui/sfml.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace { 
    class App: public imui::App
    {
        public:
            App(): imui::App(100, 200, __FILE__) {}

            void render(const imui::Region &region) override
            {
                using namespace imui;
                const Vector2 size{5, 5};
                for (int i = 0; i < 20; ++i)
                {
                    for (int j = 0; j < 40; ++j)
                    {
                        const Vector2 pos{i*size.x, j*size.y};
                        auto r = region.sub(pos, size);
                        if (r.isMouseInside())
                        {
                            S();L(region.mouse().x << " " << region.mouse().y);
                            sf::RectangleShape r;
                            r.setPosition(pos);
                            r.setSize(size);
                            r.setFillColor(sf::Color::Cyan);
                            region.draw(r);
                        }
                    }
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
