#ifndef HEADER_gubg_threading_InstanceCounter_hpp_ALREADY_INCLUDED
#define HEADER_gubg_threading_InstanceCounter_hpp_ALREADY_INCLUDED

#include <thread>
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
                    LockGuard lock(nrInstances_mutex__); 
                    ++nrInstances__;
                }
                virtual ~InstanceCounter()
                {
                    LockGuard lock(nrInstances_mutex__); 
                    --nrInstances__;
                }

                static unsigned int nrInstances()
                {
                    LockGuard lock(nrInstances_mutex__); 
                    return nrInstances__;
                }

            private:
                static unsigned int nrInstances__;
                typedef std::mutex Mutex;
                typedef std::lock_guard<Mutex> LockGuard;
                static Mutex nrInstances_mutex__;
        };
    template <typename T>
        unsigned int InstanceCounter<T>::nrInstances__;
    template <typename T>
        typename InstanceCounter<T>::Mutex InstanceCounter<T>::nrInstances_mutex__;
}

#endif
