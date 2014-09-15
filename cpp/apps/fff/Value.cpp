#include "fff/Value.hpp"
#include <cassert>
#include <sstream>

namespace  { 
	gubg::file::File emptyFile_;
	std::string emptyString_;
} 

namespace fff { 
	struct Value::Pimpl
	{
		Type type;
		gubg::file::File file;
		std::string string;
		long integer;

		Pimpl(gubg::file::File f): type(File), file(f) {}
		Pimpl(std::string s): type(String), string(s) {}
		Pimpl(long v): type(File), integer(v) {}
	};

	Value::Value(gubg::file::File file): pimpl_(new Pimpl(file)) {}
	Value::Value(std::string str): pimpl_(new Pimpl(str)) {}
	Value::Value(long v): pimpl_(new Pimpl(v)) {}

	bool Value::operator<(Value rhs) const
	{
		assert(pimpl_ && rhs.pimpl_);
		if (pimpl_->type < rhs.pimpl_->type)
			return true;
		if (rhs.pimpl_->type < pimpl_->type)
			return false;
		assert(rhs.pimpl_->type == pimpl_->type);
		switch (pimpl_->type)
		{
			case File:    return pimpl_->file    < rhs.pimpl_->file;
			case String:  return pimpl_->string  < rhs.pimpl_->string;
			case Integer: return pimpl_->integer < rhs.pimpl_->integer;
		}
		assert(false);
		return false;
	}

#define L_GET_PIMPL() assert(pimpl_); const Pimpl &pimpl = *pimpl_
	Value::Type Value::type() const
	{
		L_GET_PIMPL();
		return pimpl.type;
	}
	const gubg::file::File &Value::file() const
	{
		L_GET_PIMPL();
		if (pimpl.type != File)
			return emptyFile_;
		return pimpl.file;
	}
	const std::string &Value::string() const
	{
		L_GET_PIMPL();
		if (pimpl.type != String)
			return emptyString_;
		return pimpl.string;
	}
	long Value::integer() const
	{
		L_GET_PIMPL();
		if (pimpl.type != Integer)
			return 0;
		return pimpl.integer;
	}

	void Value::stream(std::ostream &os) const
	{
		if (!pimpl_)
		{
			os << "Invalid value";
			return;
		}
		switch (pimpl_->type)
		{
			case File:    os << "File: "    << pimpl_->file;    break;
			case String:  os << "String: "  << pimpl_->string;  break;
			case Integer: os << "Integer: " << pimpl_->integer; break;
			default:      os << "Unknown type";                 break;
		}
	}
	std::string Value::to_str() const
	{
		std::ostringstream oss;
		stream(oss);
		return oss.str();
	}
} 
