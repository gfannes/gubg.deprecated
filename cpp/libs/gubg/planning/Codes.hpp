#ifndef HEADER_gubg_planning_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg
{
	namespace planning
	{
		enum class ReturnCode
		{
			MSS_DEFAULT_CODES,
			CouldNotPlanDeadline, CouldNotPlanNormal, NotEnoughSweatAvailable,
		};
	}
}

#endif
