#ifndef HEADER_fff_Tag_hpp_ALREADY_INCLUDED
#define HEADER_fff_Tag_hpp_ALREADY_INCLUDED

#include <string>
#include <memory>
#include <ostream>

namespace fff { 
	class Tag
	{
		public:
			Tag(std::string);
			Tag(std::string, std::string);

			bool operator<(Tag) const;
			bool operator==(Tag) const;
			bool operator!=(Tag) const;

			void stream(std::ostream &) const;

		private:
			struct Pimpl;
			std::shared_ptr<const Pimpl> pimpl_;
	};
	inline std::ostream &operator<<(std::ostream &os, const Tag &t) {t.stream(os); return os;}
} 

#endif
