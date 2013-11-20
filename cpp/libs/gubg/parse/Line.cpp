#include "gubg/parse/Line.hpp"
#include <cassert>
using namespace gubg::line;
using namespace std;

namespace 
{
	size_t length(Line::End end)
	{
		switch (end)
		{
			case Line::Unix:
			case Line::OldMac:
				return 1;
			case Line::Dos:
				return 1;
		}
		return 0;
	}
}

Lines split(const string &txt)
{
	Lines lines;

	size_t ix = 0;
	while (ix < txt.size())
	{
		static const string cr_lf = "\x0d\x0a";
		const auto tmp = txt.find_first_of(cr_lf, ix);
		if (tmp == string::npos)
		{
			//No endchars found
			Line line{txt.substr(ix), Line::None};
			lines.push_back(line);
			ix = txt.size();
		}
		else
		{
			//At least one endchar found
			Line::End end = Line::None;
			switch (txt[ix])
			{
				case '\x0d':
					if (ix+1 < txt.size())
					{
						if (txt[ix+1] == '\x0a')
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
					//This should never happen
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
