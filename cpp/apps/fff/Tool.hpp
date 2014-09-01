#ifndef HEADER_fff_Tool_hpp_ALREADY_INCLUDED
#define HEADER_fff_Tool_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include <memory>
#include <vector>
#include <string>

namespace fff { 

	class Board;

	class Tool_itf
	{
		public:
			typedef std::shared_ptr<Tool_itf> Ptr;

			virtual std::string name() const = 0;
			virtual ReturnCode process(Board &) = 0;
	};

	typedef std::vector<Tool_itf::Ptr> ToolChain;
} 

#endif
