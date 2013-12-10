#include "rinle/view/Window.hpp"
#include <cassert>
using rinle::view::Window;
using namespace rinle;
using std::ostringstream;

Window::Window(const model::Model &model):
    model_(model),
    wnd_(nana::rectangle(0, 0, width_, height_))
{
    wnd_.caption(STR("Rinle"));
    wnd_.background(color::background);
    wnd_.make_event<nana::gui::events::key_char>(*this, &Window::handleCharEvent);
    wnd_.show();

    //Prepare the lines and line number labels
    const auto labelHeight = height_/nrRows_;
    for (int i = 0; i < nrRows_; ++i)
    {
        using std::make_shared;
        using nana::gui::label;
        using nana::rectangle;
        const int LineNrWidth = 30;
        {
            auto lbl = make_shared<label>(wnd_, rectangle(0, i*labelHeight, LineNrWidth, labelHeight));
            lbl->format(true);
            lbl->background(color::background);
            lineNrs_.push_back(lbl);
        }
        {
            auto lbl = make_shared<label>(wnd_, rectangle(LineNrWidth, i*labelHeight, width_, labelHeight));
            lbl->format(true);
            lbl->background(color::background);
            lines_.push_back(lbl);
        }
    }
}
void Window::handleCharEvent(const nana::gui::eventinfo &ei)
{
    assert(ei.identifier == nana::gui::events::key_char::identifier);
    notifyObservers(ei.keyboard.key);
}

void Window::convertLineNr_(nana::string &dst, int nr) const
{
    ostringstream oss;
    oss << "<size=" << fontSize_ << ", color=" << color::weak << ">" << nr << "</>";
    convert(dst, oss.str());
}
void Window::convertTokens_(nana::string &dst, const Tokens &tokens) const
{
    ostringstream oss;
    oss << "<size=" << fontSize_ << ", color=" << color::normal << ">";
    for (auto token: tokens)
    {
        bool doClose = false;
        if (false) {}
        else if (token.isSelected)   {doClose = true; oss << "<bold, color=" << color::selected << ">";}
        else if (token.isKeyword)    {doClose = true; oss << "<color=" << color::keyword << ">";}
        else if (token.isIdentifier) {doClose = true; oss << "<color=" << color::identifier << ">";}
        oss << token.range.content();
        if (doClose)
            oss << "</>";
    }
    oss << "</>";
    convert(dst, oss.str());
}
