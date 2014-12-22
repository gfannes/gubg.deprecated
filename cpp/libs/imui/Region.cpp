#include "imui/Region.hpp"

#define GUBG_MODULE "Region"
#include "gubg/log/begin.hpp"
namespace imui { 

    Region::Region(const Rect &rect, const State &state, sf::RenderWindow &rw)
        :rect_(rect), state(state), rw_(rw)
    {
    }

    float Region::width() const { return rect_.width; }
    float Region::height() const { return rect_.height; }

    bool Region::isMouseInside() const
    {
        return rect_.contains(state.mouse.position);
    }

    Region Region::sub(float x, float y, float w, float h) const
    {
        sf::FloatRect rect;
        rect_.intersects(Rect(x+rect_.left, y+rect_.top, w, h), rect);
        return Region(rect, state, rw_);
    }
    Region Region::sub(const Vector2 &pos, const Vector2 &size) const
    {
        sf::FloatRect rect;
        rect_.intersects(Rect(pos + Vector2{rect_.left, rect_.top}, size), rect);
        return Region(rect, state, rw_);
    }

} 
#include "gubg/log/end.hpp"
