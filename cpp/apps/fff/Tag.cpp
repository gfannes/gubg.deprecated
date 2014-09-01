#include "fff/Tag.hpp"
#include "gubg/OnlyOnce.hpp"
#include <vector>

namespace fff { 

	struct Tag::Pimpl
	{
		std::vector<std::string> parts;
		Pimpl(std::string str) {parts.push_back(str);}
	};

	Tag::Tag(std::string str): pimpl_(new Pimpl(str)) {}

	bool Tag::operator<(Tag rhs) const {return pimpl_->parts < rhs.pimpl_->parts;}

	void Tag::stream(std::ostream &os) const
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
