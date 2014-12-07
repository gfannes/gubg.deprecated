#ifndef HEADER_imui_Region_hpp_ALREADY_INCLUDED
#define HEADER_imui_Region_hpp_ALREADY_INCLUDED

#include "SFML/System/Vector2.hpp"

namespace imui { 

    class Region
    {
        public:
            Region(sf::Vector2f wh, sf::Vector2f mouse);

            float width() const;
            float height() const;
            const sf::Vector2f &mouse() const {return mouse_;}

            bool isMouseInside() const;

            Region sub(float x, float y, float w, float h) const;

        private:
            const sf::Vector2f wh_;
            const sf::Vector2f mouse_;
    };

} 

#endif
