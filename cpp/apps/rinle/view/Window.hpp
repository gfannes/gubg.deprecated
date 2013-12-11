#ifndef HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include "rinle/view/Util.hpp"
#include "rinle/view/Colors.hpp"
#include "rinle/view/Layout.hpp"
#include "rinle/model/Model.hpp"
#include "gubg/pattern/Observer.hpp"
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <memory>

#define GUBG_MODULE_ "Window"
#include "gubg/log/begin.hpp"
namespace rinle
{
	namespace view
	{
		inline void run() {nana::gui::exec();}

		class Window: public gubg::pattern::Observable<char>
		{
			public:
				Window(const model::Model &);

				void handleCharEvent(const nana::gui::eventinfo &);

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

            private:
                void convertLineNr_(nana::string &dst, int nr) const;
                void convertTokens_(nana::string &dst, const Tokens &tokens) const;

                const model::Model &model_;
				Layout layout_;
                nana::gui::form wnd_;
                std::vector<std::shared_ptr<nana::gui::label>> lineNrs_;
                std::vector<std::shared_ptr<nana::gui::label>> lines_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
