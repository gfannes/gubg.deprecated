#include "imui/State.hpp"
#include "SFML/Graphics.hpp"

#define GUBG_MODULE_ "State"
#include "gubg/log/begin.hpp"
namespace imui { 

    void State::process(const sf::RenderWindow &rw)
    {
        S();
        {
            auto pos = rw.getClientAreaPosition();
            L(pos);
        }
        {
            auto pos = rw.getClientAreaPosition();
            mouse.position = Vector2(sf::Mouse::getPosition()-pos);
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
