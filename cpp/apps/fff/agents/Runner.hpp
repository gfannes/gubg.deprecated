#ifndef HEADER_fff_agents_Runner_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Runner_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Agent.hpp"
#include <string>
#include <vector>

namespace fff { namespace agents { 
	class Runner: public gubg::bbs::Agent_itf
	{
		public:
			std::string name() const override {return "Runner";}
            gubg::bbs::ReturnCode process(gubg::bbs::Board &) override;

		private:
			bool do_run_ = true;
			size_t ix_;
            typedef std::vector<std::string> Arguments;
            Arguments args_;
	};
} } 

#endif
