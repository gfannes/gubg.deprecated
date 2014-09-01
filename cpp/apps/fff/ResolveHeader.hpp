#ifndef HEADER_fff_ResolveHeader_hpp_ALREADY_INCLUDED
#define HEADER_fff_ResolveHeader_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"

namespace fff { 
	class ResolveHeader: public Tool_itf
	{
		public:
			std::string name() const override {return "ResolveHeader";}
			ReturnCode process(Board &) override;
		private:
	};
} 

#endif
