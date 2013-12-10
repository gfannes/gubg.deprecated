#ifndef HEADER_rinle_Types_hpp_ALREADY_INCLUDED
#define HEADER_rinle_Types_hpp_ALREADY_INCLUDED

#include "gubg/SmartRange.hpp"
#include "gubg/file/File.hpp"
#include <vector>

namespace rinle
{
    typedef gubg::file::File File;

	struct Command
	{
		enum Type {Proceed};
		const Type type;
		int x;
		Command(Type t): type(t) {}
	};

	typedef gubg::SmartRange<std::string> Range;

	struct Token
	{
		bool isSelected = false;
		bool isKeyword = false;
		bool isIdentifier = false;
		Range range;
	};
	typedef std::vector<Token> Tokens;
	typedef std::vector<Tokens> Lines;

	struct Screen
	{
		const Lines &lines;
		size_t focus;
		Screen(const Lines &l, size_t f): lines(l), focus(f) {}
	};
}

#endif
