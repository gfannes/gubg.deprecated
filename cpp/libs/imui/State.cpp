#include "imui/State.hpp"
#include "SFML/Graphics.hpp"

namespace imui { 

    void State::process(const sf::RenderWindow &rw)
    {
        auto pos = rw.getPosition();
        mouse_ = Vector2(sf::Mouse::getPosition()-pos);
    }

} 
