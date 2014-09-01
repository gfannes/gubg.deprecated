#ifndef HEADER_fff_Options_hpp_ALREADY_INCLUDED
#define HEADER_fff_Options_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"

namespace fff { 
	struct Options
	{
		bool showHelp = false;
		gubg::file::File seed;
	};
} 

#endif
