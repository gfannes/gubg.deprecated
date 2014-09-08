#ifndef HEADER_fff_ResolveHeader_hpp_ALREADY_INCLUDED
#define HEADER_fff_ResolveHeader_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include "gubg/file/Forest.hpp"

namespace fff { 
	class ResolveHeader: public Tool_itf
	{
		public:
			ResolveHeader();

			std::string name() const override {return "ResolveHeader";}
			ReturnCode process(Board &) override;

		private:
			std::unique_ptr<const gubg::file::File> gubg_;

			size_t ix_ = 0;
			std::set<gubg::file::File> roots_;
			gubg::file::Forest forest_;
	};
} 

#endif
