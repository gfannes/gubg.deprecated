#ifndef HEADER_fff_Linker_hpp_ALREADY_INCLUDED
#define HEADER_fff_Linker_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include "gubg/OnlyOnce.hpp"

namespace fff { 
	class Linker: public Tool_itf
	{
		public:
			std::string name() const override {return "Linker";}
			ReturnCode process(Board &) override;

		private:
			gubg::OnlyOnce run_;
	};
} 

#endif
