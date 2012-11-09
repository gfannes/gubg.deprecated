#ifndef HEADER_gubg_Singleton_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Singleton_hpp_ALREADY_INCLUDED

#include <mutex>

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
                std::lock_guard<std::mutex> lock(instanceMutex__);
                if (!instance__)
                    instance__ = new T;
                return *instance__;
            }

        private:
            static T *instance__;
            static std::mutex instanceMutex__;
    };
    template <typename T>
    T *Singleton<T>::instance__ = 0;
    template <typename T>
    std::mutex Singleton<T>::instanceMutex__;
}

#endif
