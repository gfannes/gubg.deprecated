#include "rinle/view/Scroller.hpp"
#include "rinle/view/Util.hpp"
#include "rinle/view/Colors.hpp"
#include <sstream>

#define GUBG_MODULE "Scroller"
#include "gubg/log/begin.hpp"
namespace rinle { namespace view {

	void Scroller::process(std::string str)
	{
		S();L(str);

		//Scroll the labels
		if (!labels_.empty())
			for (size_t i = 0; i < labels_.size()-1; ++i)
				labels_[i]->caption(labels_[i+1]->caption());

		//Determine the color based on the first few characters
		auto clr = color::normal;
		if (false) {}
		else if (std::string("ERROR::") == str.substr(0, 7)) {str = str.substr(7); clr = color::error;}
		else if (std::string("WARNING::") == str.substr(0, 9)) {str = str.substr(9); clr = color::warning;}
		else if (std::string("OK::") == str.substr(0, 4)) {str = str.substr(4); clr = color::ok;}

		std::ostringstream oss;
		oss << "<size=" << fontSize_ << ", color=" << clr << ">" << str << "</>";
		L(oss.str());
		nana::string nstr; convert(nstr, oss.str());
		labels_.back()->caption(nstr);
	}

	void Scroller::connect(gubg::pattern::Signal<std::string> &signal)
	{
		S();
		signal.connect(slot);
	}

} }
#include "gubg/log/end.hpp"
