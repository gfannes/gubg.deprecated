#ifndef gubg_threading_InstanceCounter_hpp
#define gubg_threading_InstanceCounter_hpp

#include "boost/thread.hpp"
#include <mutex>

namespace gubg
{
    //Derive from this class to get automatic instance counting per class type
    template <typename T>
        class InstanceCounter
        {
            public:
                InstanceCounter()
                {
                    std::lock_guard<std::mutex> lock(nrInstancesMutex__); 
                    ++nrInstances__;
                }
                virtual ~InstanceCounter()
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
    template <typename T>
        unsigned int InstanceCounter<T>::nrInstances__;
    template <typename T>
        std::mutex InstanceCounter<T>::nrInstancesMutex__;
}

#endif
