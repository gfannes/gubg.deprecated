#include "fff/Value.hpp"
#include <cassert>
#include <sstream>

namespace fff { 

	Value::Value(const gubg::file::File &file): str_(file.name()) {}
	Value::Value(std::string str): str_(str) {}

	bool Value::operator<(const Value &rhs) const
	{
        return str_ < rhs.str_;
	}

	gubg::file::File Value::as_file() const
	{
        return gubg::file::File(str_);
	}
	std::string Value::as_string() const
	{
        return str_;
	}

	void Value::stream(std::ostream &os) const
	{
        os << str_;
	}
} 
