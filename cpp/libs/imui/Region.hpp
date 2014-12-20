#ifndef HEADER_imui_Region_hpp_ALREADY_INCLUDED
#define HEADER_imui_Region_hpp_ALREADY_INCLUDED

#include "imui/Types.hpp"
#include "imui/State.hpp"
#include "SFML/Graphics.hpp"

namespace imui { 

    class Region
    {
        public:
            Region(const Rect &rect, const State &state, sf::RenderWindow &rw);

            float width() const;
            float height() const;
            const Vector2 &mouse() const {return state_.mouse();}
            const KeyEvents &keys() const {return state_.keys;}
            const TextEvents &texts() const {return state_.texts;}

            bool isMouseInside() const;

            Region sub(float x, float y, float w, float h) const;
            Region sub(const Vector2 &pos, const Vector2 &size) const;

            template <typename Drawable>
                void draw(const Drawable &drawable) const {rw_.draw(drawable);}

        private:
            const Rect rect_;
            const State &state_;
            sf::RenderWindow &rw_;
    };

} 

#endif
