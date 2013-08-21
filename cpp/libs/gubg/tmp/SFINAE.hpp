#ifndef HEADER_gubg_tmp_SFINAE_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_SFINAE_hpp_ALREADY_INCLUDED

#include "gubg/tmp/If.hpp"

namespace gubg
{
    namespace tmp
    {
        struct HasMethod {};
        struct HasNotMethod {};

        template<typename T>
            struct HasUsedMemoryMethod
            {
                template<typename U, size_t (U::*)() const> struct SFINAE {};
                template<typename U> static char Test(SFINAE<U, &U::used_memory>*);
                template<typename U> static int Test(...);
                typedef typename If<sizeof(Test<T>(0)) == sizeof(char), HasMethod, HasNotMethod>::Type Value;
            };
/*
        template <typename T>
            class has_helloworld
            {
                typedef char one;
                typedef long two;

                template <typename C> static one test( typeof(&C::helloworld) ) ;
                template <typename C> static two test(...);


                public:
                enum { value = sizeof(test<T>(0)) == sizeof(char) };
            };
            */
    }
}

#endif
