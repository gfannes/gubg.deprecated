#include "imui/State.hpp"
#include "imui/Stream.hpp"
#include "SFML/Graphics.hpp"

#define GUBG_MODULE "State"
#include "gubg/log/begin.hpp"
namespace imui { 

    void State::process(const sf::RenderWindow &rw)
    {
        S();
        {
            const auto window_pos = rw.getPosition();
            const auto mouse_pos = sf::Mouse::getPosition();
            mouse.position = Vector2(mouse_pos-window_pos);
            mouse.position = Vector2(sf::Mouse::getPosition(rw));
        }
        {
            const bool lb = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            const bool rb = sf::Mouse::isButtonPressed(sf::Mouse::Right);
            mouse.left_clicked = (!mouse.left_button && lb);
            mouse.right_clicked = (!mouse.right_button && rb);
            mouse.left_button = lb;
            mouse.right_button = rb;
        }
        {
            if (hot_ids_.empty())
                hot_id_.clear();
            else
                hot_id_ = hot_ids_.back();
            hot_ids_.clear();
            L(STREAM(hot_id_));
        }
    }

    void State::setHot(const WidgetId &id) const
    {
        hot_ids_.push_back(id);
    }
    bool State::isHot(const WidgetId &id) const
    {
        if (!id.valid())
            return false;
        return id == hot_id_;
    }

} 
#include "gubg/log/end.hpp"
