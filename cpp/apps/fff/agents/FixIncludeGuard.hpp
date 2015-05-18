#ifndef HEADER_fff_agents_FixInlcudeGuard_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_FixInlcudeGuard_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"

namespace fff { namespace agents { 
	class FixIncludeGuard: public Agent_itf
	{
		public:
			std::string name() const override {return "FixIncludeGuard";}
			ReturnCode process(Board &) override;

		private:
			size_t ix_ = 0;
	};
} } 

#endif
