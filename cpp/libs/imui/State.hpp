#ifndef HEADER_imui_State_hpp_ALREADY_INCLUDED
#define HEADER_imui_State_hpp_ALREADY_INCLUDED

#include "imui/Types.hpp"
#include "SFML/Graphics.hpp"

namespace imui { 

    class State
    {
        public:
            Keys keys;
            const Vector2 &mouse() const {return mouse_;}

            void process(const sf::RenderWindow &);

        private:
            Vector2 mouse_;
    };

} 

#endif
