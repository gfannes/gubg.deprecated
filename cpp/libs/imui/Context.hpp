#ifndef HEADER_imui_Context_hpp_ALREADY_INCLUDED
#define HEADER_imui_Context_hpp_ALREADY_INCLUDED

namespace sf { 
    class RenderWindow;
} 
namespace imui { 
    class State;
} 
namespace imui { 

    class Context
    {
        public:
            Context(sf::RenderWindow &, imui::State &);
            Context(Context &&);
            ~Context();

            template <typename Drawable>
                void draw(const Drawable &drawable) const {renderWindow_.draw(drawable);}

        private:
            Context(const Context &) = delete;

            sf::RenderWindow &renderWindow_;
            imui::State &state_;
    };

} 

#endif
