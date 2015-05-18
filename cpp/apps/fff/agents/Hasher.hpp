#ifndef HEADER_fff_agents_Hasher_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Hasher_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Agent.hpp"
#include <string>
#include <set>
#include <map>

namespace fff { namespace agents { 
	class Hasher: public gubg::bbs::Agent_itf
	{
		public:
			std::string name() const override {return "Hasher";}
            gubg::bbs::ReturnCode process(gubg::bbs::Board &) override;

		private:
			size_t ix_;
            using Tags = gubg::bbs::Tags;
            Tags tags_;
            typedef std::map<gubg::bbs::Item, std::string> HashPerTV;
            HashPerTV hashPerTV_;
	};
} } 

#endif
