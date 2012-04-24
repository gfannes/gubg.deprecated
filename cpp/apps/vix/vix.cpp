#include "codes.hpp"
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"
#include "gubg/mss.hpp"
#include "SFML/Graphics.hpp"

namespace 
{
    ReturnCode handleEvents_(sf::RenderWindow &window, Controller &controller)
    {
        MSS_BEGIN(ReturnCode);
        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Closed: window.close(); break;
                default: controller.addEvent(event); break;
            }
        }
        MSS_END();
    }
    ReturnCode drawModel_(Model &model, sf::RenderWindow &window)
    {
        MSS_BEGIN(ReturnCode);
        window.clear();
        window.display();
        MSS_END();
    }
}

int main()
{
    MSS_BEGIN(int);

    Model model;
    sf::RenderWindow window(sf::VideoMode(800, 600), "ViX: VI inspired eXplorer");
    View view(model, window);
    Controller controller(model, view);

    while (window.isOpen())
    {
        handleEvents_(window, controller);
        drawModel_(model, window);
    }

    MSS_END();
}
