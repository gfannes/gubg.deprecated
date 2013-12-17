#ifndef HEADER_rinle_view_Page_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Page_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include "rinle/view/Colors.hpp"
#include "rinle/view/Font.hpp"
#include "gubg/pattern/SignalSlot.hpp"
#include "nana/gui/widgets/label.hpp"
#include <string>

namespace rinle { namespace view {

	//A page listens for a notification and will get the data via a callback
	class Page
	{
		public:
			template <typename Wnd>
				Page(Wnd &wnd, const nana::rectangle geom, double titleFrac, double borderFrac, size_t nrLines);

			typedef const PageSrc_itf &Msg;
			void process(Msg);
			gubg::pattern::Slot<Page, Msg> slot;

			void connect(gubg::pattern::Signal<Msg> &signal);

		private:
			typedef std::shared_ptr<nana::gui::label> Label;
			typedef std::vector<Label> Labels;
			const size_t titleFontSize_;
			Label title_;
			const size_t fontSize_;
			Labels border_;
			Labels body_;
	};

	//Implementation
	template <typename Wnd>
		Page::Page(Wnd &wnd, const nana::rectangle geom, double titleFrac, double borderFrac, size_t nrLines):
			slot(*this),
			titleFontSize_(font::computeSize(titleFrac*geom.height, 1)),
			fontSize_(font::computeSize((1.0-titleFrac)*geom.height, nrLines))
	{
		const int title_h = titleFrac*geom.height;
		{
			auto titleGeom = geom; titleGeom.height = title_h;
			title_.reset(new nana::gui::label(wnd, titleGeom));
			title_->format(true);
			title_->background(color::background);
		}

		{
			const int lbl_h = (1.0-titleFrac)*geom.height/nrLines;
			for (size_t i = 0; i < nrLines; ++i)
			{
				auto lblGeom = geom;
				lblGeom.y += title_h + i*lbl_h;
				lblGeom.width = borderFrac*geom.width;
				lblGeom.height = lbl_h;
				{
					auto lbl = std::make_shared<nana::gui::label>(wnd, lblGeom);
					lbl->format(true);
					lbl->background(color::background);
					border_.push_back(lbl);
				}

				lblGeom.x += lblGeom.width;
				lblGeom.width = (1.0-borderFrac)*geom.width;
				{
					auto lbl = std::make_shared<nana::gui::label>(wnd, lblGeom);
					lbl->format(true);
					lbl->background(color::background);
					body_.push_back(lbl);
				}
			}
		}
	}

} }

#endif
