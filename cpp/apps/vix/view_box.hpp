#ifndef vix_view_box_hpp
#define vix_view_box_hpp

#include "SFML/Graphics/Drawable.hpp"
#include <string>

class ViewBox: public sf::Drawable
{
    public:
        ViewBox(size_t width, size_t height):
            width_(width), height_(height){}

        void set(const std::string &str)
        {
            text_.setString(str);
        }
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
        {
            sf::RenderTexture texture;
            texture.create(width_, height_);
            texture.clear(sf::Color::Red);
            texture.draw(text_);
            texture.display();
            sf::Sprite sprite(texture.getTexture());
            target.draw(sprite);
        }
    private:
        size_t width_;
        size_t height_;
        sf::Text text_;
};

#endif
