#include "rinle/view/Page.hpp"
#include "rinle/view/Util.hpp"
#include "rinle/view/Colors.hpp"
#include <sstream>

#define GUBG_MODULE_ "Page"
#include "gubg/log/begin.hpp"
namespace rinle { namespace view {

	void Page::process(Msg pageSrc)
	{
		S();

		PageData pd; pd.lines.resize(body_.size());
		pageSrc.getPageData(pd);

		//Show the title
		{
			std::ostringstream oss; oss << "<size=" << titleFontSize_ << ", color=" << color::normal << ">" << pd.title << "</>";
			nana::string nstr; convert(nstr, oss.str());
			title_->caption(nstr);
		}

		//Show the border and body
		auto line = pd.lines.begin();
		auto border = border_.begin();
		auto body = body_.begin();
		for (; line != pd.lines.end() && border != border_.end() && body != body_.end();
				++line, ++border, ++body)
		{
			//Show the border
			{
				std::ostringstream oss;

				oss << "<size=" << fontSize_ << ", color=" << color::normal << ">" << line->first << "</>";
				nana::string nstr; convert(nstr, oss.str());
				(*border)->caption(nstr);
			}
			//Show the body
			{
				std::ostringstream oss;
				{
					oss << "<size=" << fontSize_ << ">";
					for (const auto &token: line->second)
					{
						auto clr = color::normal;
						{
							const Flags &flags = token.second;
							if (flags[Selected])
								clr = color::selected;
							else if (flags[Keyword])
								clr = color::keyword;
							else if (flags[Identifier])
								clr = color::identifier;
						}
						oss << "<color=" << clr << ">" << token.first << "</>";
					}
					oss << "</>";
				}
				nana::string nstr; convert(nstr, oss.str());
				(*body)->caption(nstr);
			}
		}
	}

	void Page::connect(gubg::pattern::Signal<Msg> &signal)
	{
		S();
		signal.connect(slot);
	}

} }
#include "gubg/log/end.hpp"
