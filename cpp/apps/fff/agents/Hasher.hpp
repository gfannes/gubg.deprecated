#ifndef HEADER_fff_agents_Hasher_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Hasher_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"
#include "fff/TagValue.hpp"
#include <string>
#include <set>
#include <map>

namespace fff { namespace agents { 
	class Hasher: public Agent_itf
	{
		public:
			std::string name() const override {return "Hasher";}
			ReturnCode process(Board &) override;

		private:
			size_t ix_;
            typedef std::set<std::string> Tags;
            Tags tags_;
            typedef std::map<TagValue, std::string> HashPerTV;
            HashPerTV hashPerTV_;
	};
} } 

#endif
