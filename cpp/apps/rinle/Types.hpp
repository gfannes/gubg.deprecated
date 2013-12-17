#ifndef HEADER_rinle_Types_hpp_ALREADY_INCLUDED
#define HEADER_rinle_Types_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/AString.hpp"
#include <vector>
#include <string>
#include <bitset>

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

	struct PageData
	{
        enum Flags { Selected, Keyword, Identifier, Nr };
        typedef gubg::AString<Flags> AString;
        typedef std::pair<std::string, std::vector<AString>> Line;
        typedef std::vector<Line> Lines;

        std::string title;
        Lines lines;
    };
    class PageSrc_itf
    {
        public:
            //Fills in the page data struct
            virtual void getPageData(PageData &) const = 0;
    };
}

#endif
