#ifndef HEADER_fff_Agent_hpp_ALREADY_INCLUDED
#define HEADER_fff_Agent_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include <memory>
#include <vector>
#include <string>

namespace fff { 

	class Board;

	class Agent_itf
	{
		public:
			typedef std::shared_ptr<Agent_itf> Ptr;

			virtual ~Agent_itf() = default;

			virtual std::string name() const = 0;
			virtual ReturnCode process(Board &) = 0;
	};

	typedef std::vector<Agent_itf::Ptr> Agents;
} 

#endif