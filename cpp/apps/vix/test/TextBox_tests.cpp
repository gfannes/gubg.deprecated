#include "../TextBox.hpp"
#include "gubg/mss.hpp"
#include "gubg/clock/timer.hpp"
#include "SFML/Graphics.hpp"

int main()
{
    MSS_BEGIN(int);

    sf::RenderWindow window(sf::VideoMode(800, 600), "TextBox tests");
    TextBox tb1(300, 200, 10);
    tb1.setPosition(sf::Vector2f(50, 50));
    tb1.set("TB1::abcdefghijklmnopqrstuvwxyz");
    TextBox tb2(300, 200, 20);
    tb2.setPosition(sf::Vector2f(450, 50));
    tb2.set("TB2");
    TextBox tb3(300, 200, 10);
    tb3.setPosition(sf::Vector2f(50, 350));
    tb3.set("TB3");
    TextBox tb4(300, 200, 10);
    tb4.setPosition(sf::Vector2f(450, 350));

    FT ft;
    ft.push_back(std::string("LE AF|"));
    ft.push_back(std::string(" "));
    ft.push_back(std::string("|LAF2|"));
    ft.push_back(std::string("_"));
    ft.push_back(std::string("|LAF2|"));
    ft.push_back(std::string("\n"));
    ft.push_back(std::string("blablabla"));
    tb4.set(ft);

    gubg::Timer timer(gubg::ResetType::NoAuto);
    while (window.isOpen())
    {
        MSS(timer.difference() < 5.0);
        window.clear();
        window.draw(tb1);
        window.draw(tb2);
        window.draw(tb3);
        window.draw(tb4);
        window.display();
    }

    MSS_END();
}
