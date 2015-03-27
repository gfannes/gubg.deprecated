#ifndef HEADER_fff_agents_Runner_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Runner_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"

namespace fff { namespace agents { 
	class Runner: public Agent_itf
	{
		public:
			std::string name() const override {return "Runner";}
			ReturnCode process(Board &) override;

		private:
			bool do_run_ = true;
			size_t ix_;
	};
} } 

#endif
