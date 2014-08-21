#ifndef HEADER_didi_Options_hpp_ALREADY_INCLUDED
#define HEADER_didi_Options_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include <ostream>

namespace didi { 
	struct Options
	{
		typedef std::vector<gubg::file::File> Directories;
		Directories directories;

		void stream(std::ostream &os) const
		{
			for (const auto &dir: directories)
				os << "Directory: " << dir << std::endl;
		}
	};
	std::ostream &operator<<(std::ostream &os, const Options &options)
	{
		options.stream(os);
		return os;
	}
} 

#endif
