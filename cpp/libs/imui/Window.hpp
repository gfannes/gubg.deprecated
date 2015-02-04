#ifndef HEADER_imui_Window_hpp_ALREADY_INCLUDED
#define HEADER_imui_Window_hpp_ALREADY_INCLUDED

#include "imui/State.hpp"
#include "imui/Context.hpp"
#include "SFML/Graphics.hpp"
#include <string>

namespace imui { 

    class Window
    {
        public:
            Window &size(unsigned int width, unsigned int height);
            Window &fullscreen();
            Window &title(const std::string &);

            Context context();

            bool operator()();

        private:
            void processEvents_();

            std::string title_;
            bool quit_ = false;
            sf::RenderWindow renderWindow_;
            imui::State state_;
    };

} 

#endif
