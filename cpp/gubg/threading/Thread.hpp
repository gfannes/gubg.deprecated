#ifndef gubg_threading_Thread_hpp
#define gubg_threading_Thread_hpp

#include "boost/thread.hpp"
#include <mutex>

namespace gubg
{
    namespace threading
    {
        //Derive from this class to get automatic thread counting per thread type
        template <typename ThreadT>
            class Thread
            {
                public:
                    Thread()
                    {
                        std::lock_guard<std::mutex> lock(nrInstancesMutex__); 
                        ++nrInstances__;
                    }
                    virtual ~Thread()
                    {
                        std::lock_guard<std::mutex> lock(nrInstancesMutex__); 
                        --nrInstances__;
                    }

                    static unsigned int nrInstances()
                    {
                        std::lock_guard<std::mutex> lock(nrInstancesMutex__); 
                        return nrInstances__;
                    }

                private:
                    static unsigned int nrInstances__;
                    static std::mutex nrInstancesMutex__;
            };
        template <typename ThreadT>
        unsigned int Thread<ThreadT>::nrInstances__;
        template <typename ThreadT>
        std::mutex Thread<ThreadT>::nrInstancesMutex__;
    }
}

#endif
