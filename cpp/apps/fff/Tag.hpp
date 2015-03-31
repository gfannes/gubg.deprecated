#ifndef HEADER_fff_Tag_hpp_ALREADY_INCLUDED
#define HEADER_fff_Tag_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>
#include <ostream>

namespace fff { 
	class Tag
	{
		public:
			Tag(std::string);
			Tag(const std::vector<std::string> &);

			bool operator<(Tag) const;
			bool operator==(Tag) const;
			bool operator!=(Tag) const;

			void stream(std::ostream &) const;
			std::string to_str() const;

		private:
            std::string str_;
	};
	inline std::ostream &operator<<(std::ostream &os, const Tag &t) {t.stream(os); return os;}
} 

#endif
