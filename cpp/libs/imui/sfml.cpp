#include "imui/sfml.hpp"

namespace imui { 

    App::App(size_t width, size_t height, const std::string &caption)
        :renderWindow_(sf::VideoMode(width, height), caption)
    {
    }

    bool App::operator()()
    {
        processEvents_();
        update_();
        render_();

        return renderWindow_.isOpen();
    }

    void App::processEvents_()
    {
        sf::Event event;
        while (renderWindow_.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                renderWindow_.close();
        }
        state_.process(renderWindow_);
    }
    void App::update_()
    {
        update();
    }
    void App::render_()
    {
        using namespace sf;
        renderWindow_.clear();
        render(Region(Rect(Vector2{}, Vector2(renderWindow_.getSize())), state_, renderWindow_));
        renderWindow_.display();
    }

} 
