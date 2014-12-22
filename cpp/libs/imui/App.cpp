#include "imui/App.hpp"

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

    void App::quitASAP()
    {
        quitASAP_ = true;
    }

    void App::processEvents_()
    {
        state_.keys.clear();
        state_.texts.clear();

        if (quitASAP_)
            renderWindow_.close();

        backend::Event event;
        while (renderWindow_.pollEvent(event))
        {
            if (event.type == backend::Event::Closed)
                renderWindow_.close();
            if (event.type == backend::Event::KeyPressed || event.type == backend::Event::KeyReleased)
                state_.keys.push_back(event);
            if (event.type == backend::Event::TextEntered)
                state_.texts.push_back(event);
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
