#ifndef gubg_thread_hpp
#define gubg_thread_hpp

#include "gubg/Platform.hpp"
#include "gubg/chrono.hpp"

#ifdef GUBG_LINUX
#include <thread>
#endif

#ifdef GUBG_MINGW
#include "boost/thread.hpp"
namespace std
{
	typedef ::boost::thread thread;
	namespace this_thread
	{
		template <typename Rep, typename Period>
			void sleep_for(const std::chrono::duration<Rep, Period> &d) { return ::boost::this_thread::sleep_for(d); }
	}
}
#endif

#endif
