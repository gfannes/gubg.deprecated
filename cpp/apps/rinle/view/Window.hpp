#ifndef HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include "rinle/view/Layout.hpp"
#include "rinle/view/Page.hpp"
#include "rinle/view/Scroller.hpp"
#include "rinle/model/Model.hpp"
#include "gubg/pattern/SignalSlot.hpp"
#include "nana/gui/widgets/form.hpp"
#include <memory>

#define GUBG_MODULE_ "Window"
#include "gubg/log/begin.hpp"
namespace rinle
{
	namespace view
	{
		class Window
		{
			public:
				Window(const model::Model &);

				gubg::pattern::Signal<char> signal;

				void handleCharEvent(const nana::gui::eventinfo &);

            private:
                const model::Model &model_;
				Layout layout_;
                nana::gui::form wnd_;

            public:
				Page page;
				Scroller scroller;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
