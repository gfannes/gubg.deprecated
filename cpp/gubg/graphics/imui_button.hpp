#ifndef gubg_graphics_imui_button_hpp
#define gubg_graphics_imui_button_hpp

#include "graphics/imui.hpp"
#include "statemachine.hpp"
#include <string>

namespace gubg
{
    //We don't actually listen to any event, we just process
    enum class Alignment {Left, Center};
    class Button: public StateMachine<bool, WidgetState>, public IWidget
    {
        public:
        Button(const TwoPoint<> &dimensions, const std::string &label, Alignment alignment, SDLCanvas &canvas):
            dimensions_(dimensions),
            label_(label),
            alignment_(alignment),
            canvas_(canvas),
            StateMachine<bool, WidgetState>(WidgetState::Emerging){}

        Button &setLabel(const std::string &label)
        {
            label_ = label;
            return *this;
        }
        Button &setFillColor(const Color &fillColor)
        {
            fillColor_ = fillColor;
            return *this;
        }
        Button resetFillColor()
        {
            fillColor_ = Color::invalid;
            return *this;
        }
        //StateMachine interface
        virtual bool processEvent(bool)
        {
            switch (state)
            {
                case WidgetState::Emerging: changeState(WidgetState::Idle); break;
                case WidgetState::Idle:
                                           if (canvas_.imui.isMouseInside(dimensions_) && canvas_.imui.checkMouseButton(MouseButton::Left, ButtonState::IsOrWentUp))
                                               changeState(WidgetState::Highlighted);
                                           break;
                case WidgetState::Highlighted://Hoovering over the button
                                           if (canvas_.imui.isMouseInside(dimensions_))
                                           {
                                               if (canvas_.imui.checkMouseButton(MouseButton::Left, ButtonState::IsOrWentDown))
                                                   changeState(WidgetState::Activating);
                                           }
                                           else
                                               changeState(WidgetState::Idle);
                                           break;
                case WidgetState::Activating://Button-down
                                           if (canvas_.imui.checkMouseButton(MouseButton::Left, ButtonState::IsOrWentUp))
                                               changeState(WidgetState::Activated);
                                           break;
                case WidgetState::Activated://Button went back up
                                           changeState(WidgetState::Idle);
                                           break;
            }
            return true;
        }
        //IWidget interface
        virtual WidgetState process()
        {
            processEvent(false);
            Style s;
            switch (state)
            {
                case WidgetState::Idle:
                    if (fillColor_.isValid())
                        s.fill(fillColor_);
                    break;
                case WidgetState::Highlighted:
                    if (fillColor_.isValid())
                        s.fill(fillColor_);
                    else
                        s.fill(Color::darkBlue);
                    break;
                case WidgetState::Activating: s.fill(Color::yellow); break;
                case WidgetState::Activated: s.fill(Color::green); break;
            }
            canvas_.drawRectangle(dimensions_, s);
            //Draw the label
            if (!label_.empty())
            {
                Style ts;
                ts.fill(Color::black).stroke(Color::white).width(2.0);
                HAlign ha;
                switch (alignment_)
                {
                    case Alignment::Center: ha = HAlign::Center; break;
                    case Alignment::Left: ha = HAlign::Left; break;
                }
                auto lh = dimensions_.height*0.75;
                auto lw = dimensions_.width - (dimensions_.height-lh);
                canvas_.drawText(label_, TwoPoint::centered(dimensions_.centerX, dimensions_.centerY, lw, lh), VAlign::Center, ha, ts);
            }
            return state;
        }

        private:
        TwoPoint<> dimensions_;
        std::string label_;
        Color fillColor_;
        Alignment alignment_;
        SDLCanvas &canvas_;
    };
}

#endif
