#ifndef HEADER_gubg_dynlib_Library_ALREADY_INCLUDED
#define HEADER_gubg_dynlib_Library_ALREADY_INCLUDED

#include "gubg/dynlib/Codes.hpp"
#include "gubg/file/File.hpp"
#include <memory>

namespace gubg { namespace dynlib { 

	class Library
	{
		public:
			Library(){}
			Library(const file::File&);

			bool valid() const {return !!pimpl_;}

            void *get(const std::string &name) const;

		private:
			ReturnCode ctor_(file::File);

			struct Pimpl;
			std::shared_ptr<Pimpl> pimpl_;
	};
} } 

#endif
