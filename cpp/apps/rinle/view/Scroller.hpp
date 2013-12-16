#ifndef HEADER_rinle_view_Scroller_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Scroller_hpp_ALREADY_INCLUDED

#include "rinle/view/Colors.hpp"
#include "rinle/view/Font.hpp"
#include "gubg/pattern/SignalSlot.hpp"
#include "nana/gui/widgets/label.hpp"
#include <string>

namespace rinle { namespace view {

	class Scroller
	{
		public:
			template <typename Wnd>
				Scroller(Wnd &wnd, const nana::rectangle geom, size_t nrLines);

			void process(std::string);
			gubg::pattern::Slot<Scroller, std::string> slot;

			void connect(gubg::pattern::Signal<std::string> &signal);

		private:
			typedef std::vector<std::shared_ptr<nana::gui::label>> Labels;
			Labels labels_;
			const size_t fontSize_;
	};

	//Implementation
	template <typename Wnd>
		Scroller::Scroller(Wnd &wnd, const nana::rectangle geom, size_t nrLines):
			slot(*this), fontSize_(font::computeSize(geom.height, nrLines))
		{
			int lbl_h = geom.height/nrLines;

			for (size_t i = 0; i < nrLines; ++i)
			{
				auto lblGeom = geom; lblGeom.y += i*lbl_h; lblGeom.height = lbl_h;
				auto lbl = std::make_shared<nana::gui::label>(wnd, lblGeom);
				lbl->format(true);
				lbl->background(color::background);
				labels_.push_back(lbl);
			}
		}

} }

#endif
