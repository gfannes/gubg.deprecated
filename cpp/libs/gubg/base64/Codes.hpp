#ifndef HEADER_gubg_base64_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_base64_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace base64 { 

	enum class ReturnCode
	{
		MSS_DEFAULT_CODES,
		IncorrectSize, InvalidEncodedChar,
	};

} } 

#endif
