#ifndef vix_view_hpp
#define vix_view_hpp

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
