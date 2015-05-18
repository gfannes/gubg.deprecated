#ifndef HEADER_fff_agents_ParseInclude_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_ParseInclude_hpp_ALREADY_INCLUDED

#include "gubg/bbs//Agent.hpp"
#include "gubg/file/Forest.hpp"

namespace fff { namespace agents { 
	class ParseInclude: public gubg::bbs::Agent_itf
	{
		public:
			ParseInclude();

			std::string name() const override {return "ParseInclude";}
            gubg::bbs::ReturnCode process(gubg::bbs::Board &) override;

		private:
			std::unique_ptr<const gubg::file::File> gubg_;
			std::unique_ptr<const gubg::file::File> imui_;
			std::unique_ptr<const gubg::file::File> lua_;
			std::unique_ptr<const gubg::file::File> catch_;
			std::unique_ptr<const gubg::file::File> sfml_;
			std::unique_ptr<const gubg::file::File> chai_;
			std::unique_ptr<const gubg::file::File> eigen_;
			std::unique_ptr<const gubg::file::File> poco_;

			size_t ix_ = 0;
	};
} } 

#endif
