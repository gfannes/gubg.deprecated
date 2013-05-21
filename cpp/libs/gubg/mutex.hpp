#ifndef HEADER_gubg_mutex_hpp_ALREADY_INCLUDED
#define HEADER_gubg_mutex_hpp_ALREADY_INCLUDED

#include "gubg/Platform.hpp"

#ifdef GUBG_LINUX
#include <mutex>
#endif

#ifdef GUBG_MINGW
#include "boost/thread/mutex.hpp"
#include "boost/thread/lock_guard.hpp"
namespace std
{
    typedef ::boost::mutex mutex;

    template <typename Lock>
	    using unique_lock = ::boost::unique_lock<Lock>;

    template <typename Lock>
	    using lock_guard = ::boost::lock_guard<Lock>;
}
#endif

#endif
