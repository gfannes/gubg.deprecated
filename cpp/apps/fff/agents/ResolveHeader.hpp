#ifndef HEADER_fff_agents_ResolveHeader_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_ResolveHeader_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Agent.hpp"
#include "gubg/file/Forest.hpp"

namespace fff { namespace agents { 
	class ResolveHeader: public gubg::bbs::Agent_itf
	{
		public:
			std::string name() const override {return "ResolveHeader";}
            gubg::bbs::ReturnCode process(gubg::bbs::Board &) override;

		private:
			size_t ix_ = 0;
			std::set<gubg::file::File> roots_;
			gubg::file::Forest forest_;
	};
} } 

#endif
