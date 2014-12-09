#ifndef HEADER_imui_sfml_hpp_ALREADY_INCLUDED
#define HEADER_imui_sfml_hpp_ALREADY_INCLUDED

#include "imui/Region.hpp"
#include "imui/State.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System/Vector2.hpp"
#include <string>

namespace imui { 

    class App
    {
        public:
            App(size_t width, size_t height, const std::string &caption);

            bool operator()();

        protected:
            virtual void update(){}

            virtual void render(const Region &){}

        private:
            void processEvents_();
            void update_();
            void render_();

            sf::RenderWindow renderWindow_;
            State state_;
    };

} 

#endif
