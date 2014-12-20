#ifndef HEADER_imui_Types_hpp_ALREADY_INCLUDED
#define HEADER_imui_Types_hpp_ALREADY_INCLUDED

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Window/Event.hpp"
#include <vector>

namespace imui { 

    typedef sf::Vector2f Vector2;
    typedef sf::FloatRect Rect;
    typedef sf::Event Event;
    typedef std::vector<Event> KeyEvents;
    typedef std::vector<Event> TextEvents;

} 

#endif
