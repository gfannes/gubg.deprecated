#ifndef HEADER_fff_agents_ParseIncludes_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_ParseIncludes_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"

namespace fff { namespace agents { 
	class ParseIncludes: public Agent_itf
	{
		public:
			std::string name() const override {return "ParseIncludes";}
			ReturnCode process(Board &) override;

		private:
			size_t ix_ = 0;
	};
} } 

#endif
