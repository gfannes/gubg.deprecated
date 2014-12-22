#include "imui/Button.hpp"

namespace imui { 

    Button::Button(const Region &region, const WidgetId &id): region_(region), id_(id)
    {
    }

    Button &Button::setPosition(const Vector2 &pos)
    {
        pos_ = pos;
        return *this;
    }
    Button &Button::setSize(const Vector2 &size)
    {
        size_ = size;
        return *this;
    }

    Event Button::operator()()
    {
        sf::RectangleShape shape;
        shape.setPosition(pos_);
        shape.setSize(size_);
        auto subregion = region_.sub(pos_, size_);
        const bool hot = subregion.isMouseInside();
        auto &state = region_.state;
        if (hot)
            state.setHot(id());

        sf::Color color = sf::Color::White;
        Event evt = Event::None;
        if (state.isHot(id()))
        {
            if (state.mouse.left_clicked)
            {
                color = sf::Color::Red;
                evt = Event::Clicked;
            }
            else
            {
                color = sf::Color::Green;
                evt = Event::Hot;
            }
        }
        shape.setFillColor(color);
        region_.draw(shape);
        return evt;
    }

} 
