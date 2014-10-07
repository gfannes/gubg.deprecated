#ifndef HEADER_gubg_toml_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_toml_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace toml { 

	enum class ReturnCode
	{
		MSS_DEFAULT_CODES,
        KeyOrPathExpected, EqualSignExpected, ValueExpected, NewlineExpected,
	};

} } 

#endif
