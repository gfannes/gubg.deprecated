#ifndef HEADER_fff_tools_Search_hpp_ALREADY_INCLUDED
#define HEADER_fff_tools_Search_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include "gubg/file/Forest.hpp"

namespace fff { namespace tools { 
	class Search: public Tool_itf
	{
		public:
			std::string name() const override {return "Search";}
			ReturnCode process(Board &) override;

		private:
			size_t ix_ = 0;
	};
} } 

#endif
