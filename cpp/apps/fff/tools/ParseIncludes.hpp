#ifndef HEADER_fff_tools_ParseIncludes_hpp_ALREADY_INCLUDED
#define HEADER_fff_tools_ParseIncludes_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"

namespace fff { namespace tools { 
	class ParseIncludes: public Tool_itf
	{
		public:
			std::string name() const override {return "ParseIncludes";}
			ReturnCode process(Board &) override;

		private:
			size_t ix_ = 0;
	};
} } 

#endif
