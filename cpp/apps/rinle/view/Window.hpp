#ifndef HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED

#include "rinle/view/Util.hpp"
#include "gubg/Platform.hpp"
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <memory>
#include <cassert>

#define GUBG_MODULE_ "Window"
#include "gubg/log/begin.hpp"
namespace rinle
{
	namespace view
	{
		inline void run() {nana::gui::exec();}

        namespace color
        {
            auto background = 0x222222;
            auto normal = 0xcccccc;
            auto weak = 0x888888;
            auto selected = 0x882288;
            auto keyword = 0xaa3333;
            auto identifier = 0x33aa33;
        }

		namespace layout
		{
#ifdef GUBG_MINGW
			const int Width = 1580;
			const int Height = 1100;
			const int FontSize = 14;
			const int NrRows = 40;
			const int FocusRow = 15;
#else
			const int Width = 1365;
			const int Height = 800;
			const int FontSize = 14;
			const int NrRows = 30;
			const int FocusRow = 15;
#endif
		}

		class Window: public gubg::pattern::Observable<char>
		{
			public:
				Window():
					width_(layout::Width), height_(layout::Height),
					wnd_(nana::rectangle(0, 0, width_, height_)),
					observer(*this)
			{
				wnd_.caption(STR("Rinle"));
				wnd_.background(color::background);
				wnd_.make_event<nana::gui::events::key_char>(*this, &Window::handleCharEvent);
				wnd_.show();

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

				template <typename Lines>
					void showLines(const Lines &lines, size_t focus)
					{
						assert(lines_.size() == lineNrs_.size());

						S();L(STREAM(lines.size(), focus));
						auto begin = lines.begin();
						int skipBefore = 0;
						if (focus < focusRow_)
							skipBefore = focusRow_ - focus;
						else
							begin += focus - focusRow_;

						auto line = lines_.begin();
						auto lineNr = lineNrs_.begin();
						for (; line != lines_.end() && lineNr != lineNrs_.end(); ++line, ++lineNr)
						{
							S();L(line - lines_.begin());
							if (skipBefore > 0)
							{
								--skipBefore;
								continue;
							}
							//Line nrs
							{
								nana::string str;
								if (begin != lines.end())
									convertLineNr_(str, begin - lines.begin());
								(*lineNr)->caption(str);
							}
							//Line itself
							{
								S();
								nana::string str;
								if (begin != lines.end())
									convertTokens_(str, *begin);
								(*line)->caption(str);
							}

							if (begin != lines.end())
								++begin;
						}
						L("end");
					}

				void notify(Screen screen) { showLines(screen.lines, screen.focus); }
				gubg::pattern::Observer_ftop<Window, Screen> observer;

				void handleCharEvent(const nana::gui::eventinfo &ei)
				{
					S();L("Received event: " << ei.identifier);
					assert(ei.identifier == nana::gui::events::key_char::identifier);
					L("This is a \'" << ei.keyboard.key << "\'");
					notifyObservers(ei.keyboard.key);
				}

			private:
				void convertLineNr_(nana::string &dst, int nr)
				{
					std::ostringstream oss;
					oss << "<size=" << fontSize_ << ", color=" << color::weak << ">" << nr << "</>";
					convert(dst, oss.str());
				}
				template <typename Tokens>
					void convertTokens_(nana::string &dst, const Tokens &tokens)
					{
						S();
						std::ostringstream oss;
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
						L(oss.str());
						convert(dst, oss.str());
					}

				const int fontSize_ = layout::FontSize;
				const int nrRows_ = layout::NrRows;
				const int focusRow_ = layout::FocusRow;
				const int width_;
				const int height_;
				nana::gui::form wnd_;
				std::vector<std::shared_ptr<nana::gui::label>> lineNrs_;
				std::vector<std::shared_ptr<nana::gui::label>> lines_;
		};
	}
}
#include "gubg/log/end.hpp"

#endif
