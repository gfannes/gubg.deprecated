#ifndef HEADER_gubg_parse_Line_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_Line_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

namespace gubg
{
	namespace line
	{
		struct Line
		{
			std::string txt;
			enum End {None, Unix, Dos, OldMac};
			End end;
		};
		const char *to_s(Line::End);

		typedef std::vector<Line> Lines;

		Lines split(const std::string &txt);

		std::string join(const Lines &);
		std::string join(const Lines &, Line::End);

		//Return false if no single end is used (None is allowed)
		bool analyseEnds(Line::End &, const Lines &);
	}
}

#endif
