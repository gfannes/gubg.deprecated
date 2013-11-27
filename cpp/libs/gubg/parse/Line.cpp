#include "gubg/parse/Line.hpp"
#include <cassert>
using namespace std;

namespace 
{
	using namespace gubg::line;

	size_t length(Line::End end)
	{
		switch (end)
		{
			case Line::Unix:
			case Line::OldMac:
				return 1;
			case Line::Dos:
				return 2;
		}
		return 0;
	}
}

#define GUBG_MODULE "line"
#include "gubg/log/begin.hpp"
namespace gubg
{
	namespace line
	{
		const char *to_s(Line::End end)
		{
			switch (end)
			{
#define L_CASE(n) case Line::n: return #n
				L_CASE(None);
				L_CASE(Unix);
				L_CASE(Dos);
				L_CASE(OldMac);
			}
			return "Unknow line end";
		}

		Lines split(const string &txt)
		{
			S();L(STREAM(txt.size()));
			Lines lines;

			size_t ix = 0;
			while (ix < txt.size())
			{
				S();
				static const string cr_lf = "\x0d\x0a";
				const auto tmp = txt.find_first_of(cr_lf, ix);
				L(STREAM(ix, tmp));
				if (tmp == string::npos)
				{
					L("No endchars found");
					Line line{txt.substr(ix), Line::None};
					lines.push_back(line);
					ix = txt.size();
				}
				else
				{
					L("At least one endchar found");
					Line::End end = Line::None;
					switch (txt[tmp])
					{
						case '\x0d':
							if (tmp+1 < txt.size())
							{
								if (txt[tmp+1] == '\x0a')
								{
									end = Line::Dos;
									break;
								}
							}
							end = Line::OldMac;
							break;
						case '\x0a':
							end = Line::Unix;
							break;
						default:
							L("This should never happen");
							assert(false);
							break;
					}
					Line line{txt.substr(ix, tmp-ix), end};
					lines.push_back(line);
					ix = tmp + length(end);
				}
			}

			return lines;
		}

		string join(const Lines &lines)
		{
			//Compute the length
			size_t len = 0;
			for (const auto &line: lines)
				len += line.txt.size() + length(line.end);

			//Allocate the final string
			string res(len, '\0');

			size_t ix = 0;
			for (const auto &line: lines)
			{
				line.txt.copy(&res[ix], string::npos);
				ix += line.txt.size();
				switch (line.end)
				{
					case Line::Unix:
						res[ix++] = '\x0a';
						break;
					case Line::Dos:
						res[ix++] = '\x0d';
						res[ix++] = '\x0a';
						break;
					case Line::OldMac:
						res[ix++] = '\x0d';
						break;
				}
			}

			return res;
		}
		string join(const Lines &lines, Line::End end)
		{
			//Compute the length
			size_t len = 0;
			const size_t endLength = length(end);
			for (const auto &line: lines)
				len += line.txt.size() + endLength;

			//Allocate the final string
			string res(len, '\0');

			size_t ix = 0;
			for (const auto &line: lines)
			{
				line.txt.copy(&res[ix], string::npos);
				ix += line.txt.size();
				switch (end)
				{
					case Line::Unix:
						res[ix++] = '\x0a';
						break;
					case Line::Dos:
						res[ix++] = '\x0d';
						res[ix++] = '\x0a';
						break;
					case Line::OldMac:
						res[ix++] = '\x0d';
						break;
				}
			}

			return res;
		}

		bool analyseEnds(Line::End &end, const Lines &lines)
		{
			Line::End lEnd = Line::None;

			for (const auto &line: lines)
			{
				if (line.end == Line::None)
					//None can be merged in other end types
					continue;
				if (lEnd == Line::None)
				{
					//No specific line end has been detected yet, this is the first
					lEnd = line.end;
					continue;
				}
				if (lEnd != line.end)
					//This line.end does not match with the previously detected line end
					return false;
			}

			end = lEnd;
			return true;
		}
	}
}
#include "gubg/log/end.hpp"
