#include "rinle/view/Page.hpp"
#include "nana/gui/widgets/form.hpp"
#include "nana/gui/wvl.hpp"
#include <string>
#include <sstream>

class Document: public rinle::view::PageSrc_itf
{
	public:
		virtual void getPageData(rinle::view::PageData &pd) const
		{
			pd.title = "Nice title here";
			int lineNr = 0;
			for (auto &line: pd.lines)
			{
				std::ostringstream oss; oss << ++lineNr;
				line.first = oss.str();

				{
					rinle::view::Token token;
					token.first = "aoeuaoeuaoeu";
					token.second[1] = true;
					line.second.push_back(token);
				}
			}
		}
		gubg::pattern::Signal<const rinle::view::PageSrc_itf &> signal;

	private:
};

int main()
{
	Document document;

	nana::gui::form window(nana::rectangle(0, 0, 500, 300));
	window.caption(STR("Page tests"));

	rinle::view::Page page(window, nana::rectangle(0, 0, 500, 300), 0.1, 0.1, 10);
	page.connect(document.signal);
	document.signal.emit(document);

    window.show();

	nana::gui::exec();

	return 0;
}
