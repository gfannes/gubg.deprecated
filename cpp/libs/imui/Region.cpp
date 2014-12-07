#include "imui/Region.hpp"
#include "SFML/Graphics/Rect.hpp"

namespace imui { 

    Region::Region(sf::Vector2f wh, sf::Vector2f mouse)
        :wh_(wh), mouse_(mouse)
    {
    }

    float Region::width() const { return wh_.x; }
    float Region::height() const { return wh_.y; }

    bool Region::isMouseInside() const
    {
        if (mouse_.x < 0)
            return false;
        if (mouse_.y < 0)
            return false;
        if (mouse_.x >= wh_.x)
            return false;
        if (mouse_.y >= wh_.y)
            return false;
        return true;
    }

    Region Region::sub(float x, float y, float w, float h) const
    {
        const sf::FloatRect mine(sf::Vector2f(), wh_);
        sf::FloatRect is;
        if (!mine.intersects(sf::FloatRect(x, y, w, h), is))
            return Region(sf::Vector2f(), sf::Vector2f());
        return Region(sf::Vector2f(is.width, is.height), mouse_ - sf::Vector2f(x, y));
    }

} 
