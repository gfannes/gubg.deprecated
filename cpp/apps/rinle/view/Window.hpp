#ifndef HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED

#include "rinle/view/Util.hpp"
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

		class Window
		{
			public:
				Window():
					width_(1365), height_(800),
					wnd_(nana::rectangle(0, 0, width_, height_))
			{
				wnd_.caption(STR("Rinle"));
				wnd_.show();

				const auto labelHeight = height_/NrRows;
				for (int i = 0; i < NrRows; ++i)
				{
					using std::make_shared;
					using nana::gui::label;
					using nana::rectangle;
					const int LineNrWidth = 30;
					lineNrs_.push_back(make_shared<label>(wnd_, rectangle(0, i*labelHeight, LineNrWidth, labelHeight)));
					lines_.push_back(  make_shared<label>(wnd_, rectangle(LineNrWidth, i*labelHeight, width_, labelHeight)));
				}
			}

				template <typename Lines, typename Iterator>
					void showLines(const Lines &lines, Iterator focus)
					{
						assert(lines_.size() == lineNrs_.size());

						S();
						Iterator begin = lines.begin();
						int skipBefore = 0;
						const auto beforeSize = focus - lines.begin();
						if (beforeSize < FocusRow)
							skipBefore = FocusRow - beforeSize;
						else
							begin += beforeSize - FocusRow;

						auto line = lines_.begin();
						auto lineNr = lineNrs_.begin();
						for (; line != lines_.end() && lineNr != lineNrs_.end(); ++line, ++lineNr)
						{
							if (skipBefore > 0)
							{
								--skipBefore;
								continue;
							}
							{
								nana::string str;
								if (begin != lines.end())
									convert(str, begin - lines.begin());
								(*lineNr)->caption(str);
							}
							{
								nana::string str;
								if (begin != lines.end())
									convert_(str, *begin);
								(*line)->caption(str);
								(*line)->foreground(0xff0000);
							}

							if (begin != lines.end())
								++begin;
						}
					}

			private:
				template <typename Tokens>
					void convert_(nana::string &dst, const Tokens &tokens)
					{
						std::ostringstream oss;
						for (auto token: tokens)
							oss << "(" << token.range.content() << ")";
						convert(dst, oss.str());
					}

				static const int NrRows = 40;
				static const int FocusRow = 15;
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
