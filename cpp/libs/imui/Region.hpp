#ifndef HEADER_imui_Region_hpp_ALREADY_INCLUDED
#define HEADER_imui_Region_hpp_ALREADY_INCLUDED

#include "imui/Types.hpp"
#include "imui/State.hpp"
#include "SFML/Graphics.hpp"

namespace imui { 

    class Region
    {
        public:
            const State &state;

            Region(const Rect &rect, const State &state, sf::RenderWindow &rw);

            float width() const;
            float height() const;
            const backend::KeyEvents &keys() const {return state.keys;}
            const backend::TextEvents &texts() const {return state.texts;}

            bool isMouseInside() const;

            Region sub(float x, float y, float w, float h) const;
            Region sub(const Vector2 &pos, const Vector2 &size) const;

            template <typename Drawable>
                void draw(const Drawable &drawable) const {rw_.draw(drawable);}

        private:
            const Rect rect_;
            sf::RenderWindow &rw_;
    };

} 

#endif
