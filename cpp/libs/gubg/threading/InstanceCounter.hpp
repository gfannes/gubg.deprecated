#ifndef gubg_threading_InstanceCounter_hpp
#define gubg_threading_InstanceCounter_hpp

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

namespace gubg
{
    //Derive from this class to get automatic instance counting per class type
    template <typename T>
        class InstanceCounter
        {
            public:
                InstanceCounter()
                {
                    boost::lock_guard<boost::mutex> lock(nrInstancesMutex__); 
                    ++nrInstances__;
                }
                virtual ~InstanceCounter()
                {
                    boost::lock_guard<boost::mutex> lock(nrInstancesMutex__); 
                    --nrInstances__;
                }

                static unsigned int nrInstances()
                {
                    boost::lock_guard<boost::mutex> lock(nrInstancesMutex__); 
                    return nrInstances__;
                }

            private:
                static unsigned int nrInstances__;
                static boost::mutex nrInstancesMutex__;
        };
    template <typename T>
        unsigned int InstanceCounter<T>::nrInstances__;
    template <typename T>
        boost::mutex InstanceCounter<T>::nrInstancesMutex__;
}

#endif
