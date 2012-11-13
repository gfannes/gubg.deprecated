#ifndef HEADER_gubg_mutex_hpp_ALREADY_INCLUDED
#define HEADER_gubg_mutex_hpp_ALREADY_INCLUDED

#include "gubg/Platform.hpp"

#ifdef GUBG_LINUX

#include <mutex>

#else//GUBG_LINUX

#include "boost/thread/mutex.hpp"

namespace std
{
    typedef ::boost::mutex mutex;
    template <typename Lock>
        struct lock_guard: public ::boost::mutex::scoped_lock
    {
        lock_guard(Lock &lock):
            ::boost::mutex::scoped_lock(lock){}
    };
}
#endif//GUBG_LINUX

#endif
