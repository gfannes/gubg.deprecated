#ifndef HEADER_fff_Codes_hpp_ALREADY_INCLUDED
#define HEADER_fff_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace fff { 
	enum class ReturnCode
	{
		MSS_DEFAULT_CODES,
		NoSeedGiven, TagValueAlreadyExists, AlreadyStarted, NoNewEntries, HashAlreadySet,
		AgentAlreadyPresent, CompileFailure, LinkFailure, UnknownOption, UnknownCommand,
	};
} 

#endif