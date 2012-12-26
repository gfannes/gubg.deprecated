#include "gubg/graphics/imui_button.hpp"
#include "gubg/l.hpp"
using namespace gubg;

Button::Button(const TwoPoint<> &dimensions, const std::string &label, Alignment alignment, SDLCanvas &canvas):
    StateHolder(WidgetState::Emerging),
    dimensions_(dimensions),
    label_(label),
    alignment_(alignment),
    canvas_(canvas),
    imui_(canvas.getIMUI())
{
    L("Button ctor: " << std::hex << this);
}
Button::~Button()
{
    L("Button dtor: " << std::hex << this);
}

Button &Button::setLabel(const std::string &label)
{
    label_ = label;
    return *this;
}
Button &Button::setFillColor(const Color &fillColor)
{
    fillColor_ = fillColor;
    return *this;
}
Button &Button::resetFillColor()
{
    fillColor_ = Color::invalid;
    return *this;
}
//StateMachine interface
bool Button::processEvent_(bool)
{
    switch (state())
    {
        case WidgetState::Emerging: changeState(WidgetState::Idle); break;
        case WidgetState::Idle:
                                    if (imui_->isMouseInside(dimensions_) && imui_->checkMouseButton(MouseButton::Left, ButtonState::IsOrWentUp))
                                        changeState(WidgetState::Highlighted);
                                    break;
        case WidgetState::Highlighted://Hoovering over the button
                                    if (imui_->isMouseInside(dimensions_))
                                    {
                                        if (imui_->checkMouseButton(MouseButton::Left, ButtonState::IsOrWentDown))
                                            changeState(WidgetState::Activating);
                                    }
                                    else
                                        changeState(WidgetState::Idle);
                                    break;
        case WidgetState::Activating://Button-down
                                    if (imui_->checkMouseButton(MouseButton::Left, ButtonState::IsOrWentUp))
                                        changeState(WidgetState::Activated);
                                    break;
        case WidgetState::Activated://Button went back up
                                    changeState(WidgetState::Idle);
                                    break;
    }
    return true;
}
//IWidget interface
WidgetState Button::process()
{
    processEvent_(false);
    Style s;
    switch (state())
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
        auto lh = dimensions_.height()*0.75;
        auto lw = dimensions_.width() - (dimensions_.height()-lh);
        canvas_.drawText(label_, TwoPoint<>::centered(dimensions_.centerX(), dimensions_.centerY(), lw, lh), VAlign::Center, ha, ts);
    }
    return state();
}
