#ifndef vix_TextBox_hpp
#define vix_TextBox_hpp

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics.hpp"
#include "gubg/tree/node.hpp"
#include <string>

struct Style
{
    sf::Color color;
    Style():
        color(sf::Color::Green){}
};
typedef gubg::Node<Style, std::string> FT;

class TextBox: public sf::Transformable, public sf::Drawable
{
    public:
        TextBox(size_t width, size_t height, size_t nrLines):
            width_(width), height_(height)
    {
        setNrLines(nrLines);
    }

        TextBox &setNrLines(size_t nrLines)
        {
            const float cs = float(height_)/(nrLines+0.3);
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
        void set(const FT &ft)
        {
            size_t ix = 0;
            for (auto it = ft.begin(); it != ft.end(); ++it)
            {
                const auto& n = *it;
                if (n.leaf)
                    texts_[ix++].setString(*n.leaf);
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
            states.transform *= getTransform();
            target.draw(sprite, states);
        }
    private:
        size_t width_;
        size_t height_;
        typedef std::vector<sf::Text> Texts;
        Texts texts_;
};

#endif
