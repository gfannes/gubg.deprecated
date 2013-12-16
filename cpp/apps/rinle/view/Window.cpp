#include "rinle/view/Window.hpp"
#include <cassert>
using rinle::view::Window;
using namespace rinle;
using std::ostringstream;

namespace 
{
	const double pageFrac = 0.8;
	const double scrollerFrac = (1.0-pageFrac);
}

Window::Window(const model::Model &model):
    model_(model),
    wnd_(nana::rectangle(0, 0, layout_.width, layout_.height)),
	page(wnd_, nana::rectangle(0, 0, layout_.width, pageFrac*layout_.height), 0.2, 0.1, 40),
	scroller(wnd_, nana::rectangle(0, pageFrac*layout_.height, layout_.width, scrollerFrac*layout_.height), 6)
{
    wnd_.caption(STR("Rinle"));
    wnd_.background(color::background);
    wnd_.make_event<nana::gui::events::key_char>(*this, &Window::handleCharEvent);
    wnd_.show();
}
void Window::handleCharEvent(const nana::gui::eventinfo &ei)
{
    assert(ei.identifier == nana::gui::events::key_char::identifier);
    signal.emit(ei.keyboard.key);
}

/*
void Window::convertLineNr_(nana::string &dst, int nr) const
{
    ostringstream oss;
    oss << "<size=" << layout_.fontSize << ", font=\"" << layout_.font << "\", color=" << color::weak << ">" << nr << "</>";
    convert(dst, oss.str());
}
void Window::convertTokens_(nana::string &dst, const Tokens &tokens) const
{
    ostringstream oss;
    oss << "<size=" << layout_.fontSize << ", font=\"" << layout_.font << "\", color=" << color::normal << ">";
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
*/
