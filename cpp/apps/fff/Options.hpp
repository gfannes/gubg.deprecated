#ifndef HEADER_fff_Options_hpp_ALREADY_INCLUDED
#define HEADER_fff_Options_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

namespace fff { 
	struct Options
	{
		bool showHelp = false;
		typedef std::vector<std::string> Seeds;
		Seeds seeds;
	};
} 

#endif
