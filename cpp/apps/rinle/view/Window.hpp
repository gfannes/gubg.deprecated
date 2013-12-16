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

				/*
                void show(const File &file, size_t focus)
                {
                    S();L(STREAM(file, focus));
                    size_t startLine = 0;
                    size_t emptyLines = 0;
                    if (focus > layout_.focusRow)
                        startLine = (focus - layout_.focusRow);
                    else
                        emptyLines = (layout_.focusRow - focus);
                    size_t nrLines = (layout_.nrRows - layout_.focusRow) + (focus - startLine);
                    Lines lines;
                    model_.getLines(lines, file, startLine, nrLines);

                    auto line = lines_.begin();
                    auto lineNr = lineNrs_.begin();
                    auto begin = lines.begin();
                    for (; line != lines_.end() && lineNr != lineNrs_.end(); ++line, ++lineNr)
                    {
                        if (emptyLines > 0)
                        {
                            --emptyLines;
                            nana::string str;
                            (*lineNr)->caption(str);
                            (*line)->caption(str);
                            continue;
                        }
                        //Line nrs
                        {
                            nana::string str;
                            if (begin != lines.end())
                                convertLineNr_(str, startLine + (begin - lines.begin()));
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
				*/

            private:
				/*
                void convertLineNr_(nana::string &dst, int nr) const;
                void convertTokens_(nana::string &dst, const Tokens &tokens) const;
				*/

                const model::Model &model_;
				Layout layout_;
                nana::gui::form wnd_;
				Page page_;
				Scroller scroller_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
