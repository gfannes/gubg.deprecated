#ifndef aoeuaoeu
#define aoeuaoeu

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

		typedef std::vector<Line> Lines;

		Lines split(const std::string &txt);

		std::string join(const Lines &);
		std::string join(const Lines &, Line::End);
	}
}

#endif
