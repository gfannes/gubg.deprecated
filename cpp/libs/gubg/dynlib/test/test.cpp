#include "gubg/dynlib/Visibility.hpp"

extern "C"
{
	GUBG_DYNLIB_EXPORT int answer()
	{
		return 42;
	}
}
