#ifndef vix_text_box_hpp
#define vix_text_box_hpp

#include "SFML/Graphics/Drawable.hpp"
#include "gubg/tree/node.hpp"
#include <string>

class TextBox: public sf::Drawable
{
    public:
        TextBox(size_t width, size_t height, size_t nrLines):
            width_(width), height_(height)
    {
        setNrLines(nrLines);
    }

        TextBox &setNrLines(size_t nrLines)
        {
            const float cs = float(height_)/nrLines;
            texts_.resize(nrLines);
            for (auto t = texts_.begin(); t != texts_.end(); ++t)
            {
                t->setCharacterSize(cs);
                t->setPosition(0.0, cs*(t - texts_.begin()));
            }
            return *this;
        }

        void set(const std::string &str)
        {
            for (auto t = texts_.begin(); t != texts_.end(); ++t)
            {
                t->setString(str);
            }
        }

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
        {
            sf::RenderTexture texture;
            texture.create(width_, height_);
            texture.clear(sf::Color::Red);
            for (auto t = texts_.begin(); t != texts_.end(); ++t)
                texture.draw(*t);
            texture.display();
            sf::Sprite sprite(texture.getTexture());
            target.draw(sprite);
        }
    private:
        size_t width_;
        size_t height_;
        typedef std::vector<sf::Text> Texts;
        Texts texts_;
};

#endif
