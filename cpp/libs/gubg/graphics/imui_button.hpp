#ifndef gubg_graphics_imui_button_hpp
#define gubg_graphics_imui_button_hpp

#include "graphics/imui.hpp"
#include "graphics/sdl_canvas.hpp"
#include "statemachine.hpp"
#include <string>

namespace gubg
{
    //We don't actually listen to any event, we just process
    enum class Alignment {Left, Center};
    class Button: public StateMachine<bool, WidgetState>, public IWidget
    {
        public:
            Button(const TwoPoint<> &dimensions, const std::string &label, Alignment alignment, SDLCanvas &canvas);
            virtual ~Button();

            //Chainable setters
            Button &setLabel(const std::string &label);
            Button &setFillColor(const Color &fillColor);
            Button &resetFillColor();

            //StateMachine interface
            virtual bool processEvent(bool);

            //IWidget interface
            virtual WidgetState process();

        private:
            TwoPoint<> dimensions_;
            std::string label_;
            Color fillColor_;
            Alignment alignment_;
            SDLCanvas &canvas_;
            SDLCanvas::IMUIPtr imui_;
    };
}

#endif
