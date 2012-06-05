#ifndef vix_TextBox_hpp
#define vix_TextBox_hpp

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics.hpp"
#include "gubg/tree/tree.hpp"
#include "gubg/string_algo.hpp"
#include <string>

#include <iostream>
#define L(m) std::cout<<m<<std::endl

struct Style
{
    sf::Color color;
    Style():
        color(sf::Color::Green){}
};
typedef gubg::Tree<std::string, Style>::Hub FT;

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
            texts_.resize(nrLines);
            return *this;
        }

        sf::Text createText_(size_t ix) const
        {
            float cs = float(height_)/(texts_.size()+0.3);
            //We cache the character width computed from '_' that we will use for a ' '
            //TODO::Make this thread-safe and upgrade the cache to something that can handle more than 1 character size
            static float cw_ = 0.0;
            {
                static float cs_ = 0.0;
                if (cs != cs_)
                {
                    cs_ = cs;
                    auto t = sf::Text("_");
                    t.setCharacterSize(cs);
                    cw_ = t.getLocalBounds().width;
                    L("cw_: " << cw_);
                }
            }
            sf::Text text;
            text.setCharacterSize(cs);
            float xpos = 0.0;
            if (!texts_[ix].empty())
            {
                const auto &t = texts_[ix].back();
                auto str = t.getString().toAnsiString();
                auto nrWhiteSpace = gubg::string_algo::nrLeading(' ', str) + gubg::string_algo::nrTrailing(' ', str);
                L("nr ws: " << nrWhiteSpace);
                auto width = t.getLocalBounds().width + cw_*nrWhiteSpace;
                L("width: " << width);
                xpos = t.getPosition().x + width;
            }
            text.setPosition(xpos, cs*ix);
            return text;
        }
        void clearTexts_()
        {
            for (auto t = texts_.begin(); t != texts_.end(); ++t)
                t->clear();
        }
        void set(const std::string &str)
        {
            clearTexts_();
            for (auto t = texts_.begin(); t != texts_.end(); ++t)
            {
                auto text = createText_(t-texts_.begin());
                text.setString(str);
                t->push_back(text);
            }
        }
        void set(const FT &ft)
        {
            clearTexts_();
            size_t ix = 0;
            const FT *style = &ft;
            for (auto it = ft.begin(); it != ft.end(); ++it)
            {
                auto newStyle = &it.parent();
                if (style != newStyle)
                    style = newStyle;

                auto str = it.leaf();
                if (!str.empty())
                {
                    if (str == "\n")
                        ++ix;
                    else
                    {
                        auto text = createText_(ix);
                        text.setString(str);
                        texts_[ix].push_back(text);
                    }
                }
            }
        }

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
        {
            std::unique_ptr<sf::Texture> texture2;
            {
                sf::RenderTexture texture;
                texture.create(width_, height_);
                texture.clear(sf::Color::Red);
                for (auto ts = texts_.begin(); ts != texts_.end(); ++ts)
                {
                    for (auto t = ts->begin(); t != ts->end(); ++t)
                        texture.draw(*t);
                }
                texture.display();
                //    sf::Sprite sprite(texture.getTexture());
                texture2.reset(new sf::Texture(texture.getTexture()));
            }
            sf::Sprite sprite(*texture2);
            states.transform *= getTransform();
            target.draw(sprite, states);
        }
    private:
        size_t width_;
        size_t height_;
        typedef std::vector<std::vector<sf::Text>> Texts;
        Texts texts_;
};

#endif
