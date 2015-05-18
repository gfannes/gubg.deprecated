#ifndef HEADER_fff_agents_Starter_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Starter_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Agent.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/file/File.hpp"
#include "fff/Codes.hpp"

namespace fff { namespace agents { 

	class Starter: public gubg::bbs::Agent_itf
	{
		public:
            using Board = gubg::bbs::Board;

			std::string name() const override {return "Starter";}
            gubg::bbs::ReturnCode process(gubg::bbs::Board &) override;

		private:
			ReturnCode processFileRef_(Board &, const gubg::file::File &);
			ReturnCode processCommand_(Board &, const std::string &);
			ReturnCode processOption_(Board &, const std::string &);

			gubg::OnlyOnce addExeChain_;
			size_t ix_ = 0;
	};

} } 

#endif
