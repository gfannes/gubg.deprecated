#ifndef gubg_Singleton_hpp
#define gubg_Singleton_hpp

#include "boost/thread/mutex.hpp"

namespace gubg
{
    template <typename T>
    class Singleton
    {
        public:
            static T &instance()
            {
                boost::mutex::scoped_lock lock(instanceMutex__);
                if (!instance__)
                    instance__ = new T;
                return *instance__;
            }

        private:
            static T *instance__;
            static boost::mutex instanceMutex__;
    };
    template <typename T>
    T *Singleton<T>::instance__ = 0;
    template <typename T>
    boost::mutex Singleton<T>::instanceMutex__;
}

#endif
