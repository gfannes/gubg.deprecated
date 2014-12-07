#ifndef HEADER_fff_tools_ResolveHeader_hpp_ALREADY_INCLUDED
#define HEADER_fff_tools_ResolveHeader_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include "gubg/file/Forest.hpp"

namespace fff { namespace tools { 
	class ResolveHeader: public Tool_itf
	{
		public:
			ResolveHeader();

			std::string name() const override {return "ResolveHeader";}
			ReturnCode process(Board &) override;

		private:
			std::unique_ptr<const gubg::file::File> gubg_;
			std::unique_ptr<const gubg::file::File> imui_;
			std::unique_ptr<const gubg::file::File> lua_;
			std::unique_ptr<const gubg::file::File> catch_;
			std::unique_ptr<const gubg::file::File> sfml_;
			std::unique_ptr<const gubg::file::File> chai_;
			std::unique_ptr<const gubg::file::File> eigen_;

			size_t ix_ = 0;
			std::set<gubg::file::File> roots_;
			gubg::file::Forest forest_;
	};
} } 

#endif
