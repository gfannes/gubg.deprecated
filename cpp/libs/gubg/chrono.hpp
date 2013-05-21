#ifndef gubg_chrono_hpp
#define gubg_chrono_hpp

#include "gubg/Platform.hpp"

#ifdef GUBG_LINUX
#include <chrono>
#endif

#ifdef GUBG_MINGW
#include "boost/chrono.hpp"
namespace std
{
	namespace chrono
	{
		typedef ::boost::chrono::high_resolution_clock high_resolution_clock;
		typedef ::boost::chrono::milliseconds milliseconds;

		template <typename Rep, typename Period>
			using duration = ::boost::chrono::duration<Rep, Period>;

		template <class ToDuration, class Rep, class Period>
			constexpr ToDuration duration_cast(const duration<Rep,Period>& d) { return ::boost::chrono::duration_cast(d); }
		
	}
}
#endif

#endif

