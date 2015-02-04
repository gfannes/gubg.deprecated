#include "imui/Context.hpp"
#include "imui/State.hpp"
#include "SFML/Graphics.hpp"

namespace imui { 

    Context::Context(sf::RenderWindow &rw, imui::State &state):
        renderWindow_(rw), state_(state)
    {
        renderWindow_.clear();
    }
    Context::~Context()
    {
        renderWindow_.display();
    }

    Context::Context(Context &&dying):
        renderWindow_(dying.renderWindow_), state_(dying.state_)
    {
    }

} 
