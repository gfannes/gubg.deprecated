#ifndef gubg_Singleton_hpp
#define gubg_Singleton_hpp

#include "boost/thread/mutex.hpp"

//Insert this into the class definition to
// * make its constructors private
// * provide a static instance()-method
//The default constructor still needs an implementation
#define GUBG_SINGLETON(Type) \
        private: \
            friend class gubg::Singleton<Type>; \
            Type(); \
            Type(const Type &); \
            Type &operator=(const Type &); \
        public: \
            static Type &instance(){return gubg::Singleton<Type>::instance();} \
        private:

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
