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
            static Type &instance_meyers(){return gubg::Singleton<Type>::instance_meyers();} \
        private:

namespace gubg
{
    template <typename T>
    class Singleton
    {
        public:
            //Use before main to get around the static initialization order fiasco
            static T &instance_meyers()
            {
                static T instance_;
                return instance_;
            }
            static T &instance()
            {
                std::lock_guard<std::mutex> lock(instanceMutex__);
                if (!instance__)
                    instance__ = &instance_meyers();
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
