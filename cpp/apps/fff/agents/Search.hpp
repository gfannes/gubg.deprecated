#ifndef HEADER_fff_agents_Search_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Search_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"
#include "gubg/file/Forest.hpp"

namespace fff { namespace agents { 
	class Search: public Agent_itf
	{
		public:
			std::string name() const override {return "Search";}
			ReturnCode process(Board &) override;

		private:
			size_t ix_ = 0;
	};
} } 

#endif
