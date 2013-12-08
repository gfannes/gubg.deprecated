#ifndef HEADER_vix_view_hpp_ALREADY_INCLUDED
#define HEADER_vix_view_hpp_ALREADY_INCLUDED

#include "SFML/Graphics.hpp"

class View
{
    public:
        View(Model &model, sf::RenderWindow &window):
            model_(model), window_(window){}

    private:
        Model &model_;
        sf::RenderWindow &window_;
};

#endif
