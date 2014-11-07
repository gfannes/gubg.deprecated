#ifndef HEADER_fff_tools_Starter_hpp_ALREADY_INCLUDED
#define HEADER_fff_tools_Starter_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/file/File.hpp"

namespace fff { namespace tools { 

	class Starter: public Tool_itf
	{
		public:
			std::string name() const override {return "Starter";}
			ReturnCode process(Board &) override;

		private:
			ReturnCode processFile_(Board &, const gubg::file::File &);
			ReturnCode processOption_(Board &, const std::string &);

			gubg::OnlyOnce addExeChain_;
			size_t ix_ = 0;
	};

} } 

#endif
