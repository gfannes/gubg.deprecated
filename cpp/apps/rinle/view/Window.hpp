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

        namespace color
        {
            auto background = 0x222222;
            auto normal = 0xcccccc;
            auto selected = 0x224422;
            auto keyword = 0xaa3333;
            auto identifier = 0x33aa33;
        }

		class Window
		{
			public:
				Window():
					width_(1365), height_(800),
					wnd_(nana::rectangle(0, 0, width_, height_))
			{
				wnd_.caption(STR("Rinle"));
                wnd_.background(color::background);
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

				template <typename Lines, typename Iterator>
					void showLines(const Lines &lines, Iterator focus)
					{
						assert(lines_.size() == lineNrs_.size());

						S();
						Iterator begin = lines.begin();
						int skipBefore = 0;
						const auto beforeSize = focus - lines.begin();
						if (beforeSize < focusRow_)
							skipBefore = focusRow_ - beforeSize;
						else
							begin += beforeSize - focusRow_;

						auto line = lines_.begin();
						auto lineNr = lineNrs_.begin();
						for (; line != lines_.end() && lineNr != lineNrs_.end(); ++line, ++lineNr)
						{
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
								nana::string str;
								if (begin != lines.end())
									convertTokens_(str, *begin);
								(*line)->caption(str);
							}

							if (begin != lines.end())
								++begin;
						}
					}

			private:
                void convertLineNr_(nana::string &dst, int nr)
                {
                    std::ostringstream oss;
                    oss << "<size=" << fontSize_ << ">" << nr << "</>";
                    convert(dst, oss.str());
                }
				template <typename Tokens>
					void convertTokens_(nana::string &dst, const Tokens &tokens)
					{
						std::ostringstream oss;
                        oss << "<size=" << fontSize_ << ", color=" << color::normal << ">";
						for (auto token: tokens)
                        {
                            bool doClose = false;
                            if (false) {}
                            else if (token.isSelected)   {doClose = true; oss << "<color=" << color::selected << ">";}
                            else if (token.isKeyword)    {doClose = true; oss << "<color=" << color::keyword << ">";}
                            else if (token.isIdentifier) {doClose = true; oss << "<color=" << color::identifier << ">";}
							oss << token.range.content();
                            if (doClose)
                                oss << "</>";
                        }
                        oss << "</>";
						convert(dst, oss.str());
					}

                const int fontSize_ = 14;
				const int nrRows_ = 30;
				const int focusRow_ = 15;
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
