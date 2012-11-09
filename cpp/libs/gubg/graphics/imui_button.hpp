#ifndef HEADER_gubg_graphics_imui_button_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graphics_imui_button_hpp_ALREADY_INCLUDED

#include "gubg/graphics/imui.hpp"
#include "gubg/graphics/sdl_canvas.hpp"
//#include "gubg/statemachine/StateMachine.hpp"
#include <string>

namespace gubg
{
    struct StateHolder
    {
        StateHolder(WidgetState init):
            state_(init){}
        void changeState(WidgetState newState){state_ = newState;}
        WidgetState state() const {return state_;}
        WidgetState state_;
    };

    //We don't actually listen to any event, we just process
    enum class Alignment {Left, Center};
    class Button: public IWidget, StateHolder
    {
        public:
            Button(const TwoPoint<> &dimensions, const std::string &label, Alignment alignment, SDLCanvas &canvas);
            virtual ~Button();

            //Chainable setters
            Button &setLabel(const std::string &label);
            Button &setFillColor(const Color &fillColor);
            Button &resetFillColor();

            //StateMachine interface
            virtual bool processEvent_(bool);

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
