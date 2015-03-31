#include "fff/Tag.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/string_algo.hpp"
#include <vector>
#include <sstream>
using namespace std;

namespace fff { 

	Tag::Tag(string str): str_(str) {}
	Tag::Tag(const vector<string> &parts): str_(gubg::string_algo::join(parts, '.')) {}

	bool Tag::operator<(Tag rhs) const {return str_ < rhs.str_;}
	bool Tag::operator==(Tag rhs) const {return str_ == rhs.str_;}
	bool Tag::operator!=(Tag rhs) const {return !operator==(rhs);}

	void Tag::stream(ostream &os) const
	{
        os << str_;
	}
	std::string Tag::to_str() const
	{
        return str_;
	}
} 
