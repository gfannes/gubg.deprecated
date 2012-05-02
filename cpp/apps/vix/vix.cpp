#include "codes.hpp"
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"
#include "text_box.hpp"
#include "gubg/mss.hpp"
#include "gubg/sleep/sleep.hpp"
#include "SFML/Graphics.hpp"
#include <string>
using namespace std;

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
    const int FontSize = 15;
    ReturnCode drawModel_(Model &model, sf::RenderWindow &window)
    {
        MSS_BEGIN(ReturnCode);
        string str;
        for (auto ch = 'a'; ch <= 'z'; ++ch)
            str.push_back(ch);
        for (auto ch = 'A'; ch <= 'Z'; ++ch)
            str.push_back(ch);
        TextBox vb(400, 400, 20);
        vb.set(str);

        window.clear();
        window.draw(vb);
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

    int i = 0;
    while (window.isOpen())
    {
        cout << ++i << endl;
        handleEvents_(window, controller);
        drawModel_(model, window);
        gubg::nanosleep(0, 5000000000);
    }

    MSS_END();
}
