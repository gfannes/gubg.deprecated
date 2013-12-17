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
					for (const auto &astr: line->second)
					{
						auto clr = color::normal;
                        bool bold = true;
						{
							const auto &flags = astr.flags;
							if (flags[PageData::Identifier])
								clr = color::identifier;
							if (flags[PageData::Keyword])
								clr = color::keyword;
							if (flags[PageData::Locus])
								clr = color::selected;
                            if (flags[PageData::Selected])
                                bold = true;
						}
						oss << "<color=" << clr << (bold ? ", bold=true" : "") << ">" << astr << "</>";
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
