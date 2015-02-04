#include "imui/Window.hpp"

namespace imui { 

    Window &Window::size(unsigned int width, unsigned int height)
    {
        renderWindow_.create(sf::VideoMode(width, height), title_);
        return *this;
    }
    Window &Window::fullscreen()
    {
        renderWindow_.create(sf::VideoMode::getDesktopMode(), title_);
        return *this;
    }
    Window &Window::title(const std::string &title)
    {
        title_ = title;
        renderWindow_.setTitle(title);
        return *this;
    }

    bool Window::operator()()
    {
        processEvents_();
        return renderWindow_.isOpen();
    }

    Context Window::context()
    {
        Context ctx(renderWindow_, state_);
        return ctx;
    }

    void Window::processEvents_()
    {
        state_.keys.clear();
        state_.texts.clear();

        if (quit_)
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

} 
