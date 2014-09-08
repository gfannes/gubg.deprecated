#ifndef HEADER_fff_Starter_hpp_ALREADY_INCLUDED
#define HEADER_fff_Starter_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"

namespace fff { 
	class Starter: public Tool_itf
	{
		public:
			std::string name() const override {return "Starter";}
			ReturnCode process(Board &) override;

		private:
			bool started_ = false;
	};
} 

#endif
