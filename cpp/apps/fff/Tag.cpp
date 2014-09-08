#include "fff/Tag.hpp"
#include "gubg/OnlyOnce.hpp"
#include <vector>
using namespace std;

namespace fff { 

	struct Tag::Pimpl
	{
		vector<string> parts;
		Pimpl(string str) {parts.push_back(str);}
		Pimpl(string str1, string str2) {parts.push_back(str1); parts.push_back(str2);}
	};

	Tag::Tag(string str): pimpl_(new Pimpl(str)) {}
	Tag::Tag(string str1, string str2): pimpl_(new Pimpl(str1, str2)) {}

	bool Tag::operator<(Tag rhs) const {return pimpl_->parts < rhs.pimpl_->parts;}
	bool Tag::operator==(Tag rhs) const {return pimpl_->parts == rhs.pimpl_->parts;}
	bool Tag::operator!=(Tag rhs) const {return pimpl_->parts != rhs.pimpl_->parts;}

	void Tag::stream(ostream &os) const
	{
		if (!pimpl_)
		{
			os << "Invalid value";
			return;
		}
		gubg::OnlyOnce skipDot;
		for (const auto &part: pimpl_->parts)
		{
			if (!skipDot())
				os << ".";
			os << part;
		}
	}
} 
