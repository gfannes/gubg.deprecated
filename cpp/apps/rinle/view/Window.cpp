#include "rinle/view/Window.hpp"
#include <cassert>
using rinle::view::Window;
using namespace rinle;
using std::ostringstream;

namespace 
{
	const double pageFrac = 0.9;
	const double scrollerFrac = (1.0-pageFrac);
}

Window::Window(const model::Model &model):
    model_(model),
    wnd_(nana::rectangle(0, 0, layout_.width, layout_.height)),
	page(wnd_, nana::rectangle(0, 0, layout_.width, pageFrac*layout_.height), 0.05, 0.05, layout_.nrRows),
	scroller(wnd_, nana::rectangle(0, pageFrac*layout_.height, layout_.width, scrollerFrac*layout_.height), 3)
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
