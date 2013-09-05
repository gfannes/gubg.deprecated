#ifndef HEADER_gubg_tmp_SFINAE_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_SFINAE_hpp_ALREADY_INCLUDED

#include "gubg/tmp/If.hpp"

namespace gubg
{
    namespace tmp
    {
        struct HasMethod {};
        struct HasNotMethod {};

        //method_signature: size_t (U::*)() const, the U is important
#define GUBG_CHECK_FOR_METHOD(test_name, method_name, method_signature) \
        template<typename ObjectType_> \
            struct test_name \
            { \
                template<typename U, method_signature> struct SFINAE {}; \
                template<typename U> static char Test(SFINAE<U, &U::method_name>*); \
                template<typename U> static int Test(...); \
                typedef typename gubg::tmp::If<sizeof(Test<ObjectType_>(0)) == sizeof(char), gubg::tmp::HasMethod, gubg::tmp::HasNotMethod>::Type Value; \
            }
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
