#ifndef HEADER_fff_Types_hpp_ALREADY_INCLUDED
#define HEADER_fff_Types_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include <string>

namespace fff { 
	typedef gubg::file::File File;
	typedef std::string Tag;
	struct TaggedFile
	{
		typedef std::shared_ptr<TaggedFile> Ptr;
		typedef std::shared_ptr<const TaggedFile> CPtr;
		File file;
		Tag tag;
	};
} 

#endif
