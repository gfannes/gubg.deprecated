#ifndef HEADER_fff_agents_ExtractIncludes_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_ExtractIncludes_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Agent.hpp"

namespace fff { namespace agents { 
	class ExtractIncludes: public gubg::bbs::Agent_itf
	{
		public:
			std::string name() const override {return "ExtractIncludes";}
            gubg::bbs::ReturnCode process(gubg::bbs::Board &) override;

		private:
			size_t ix_ = 0;
	};
} } 

#endif
