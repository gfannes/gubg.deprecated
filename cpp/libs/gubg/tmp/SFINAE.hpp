#ifndef gubg_tmp_SFINAE_hpp
#define gubg_tmp_SFINAE_hpp

namespace gubg
{
    namespace tmp
    {
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
    }
}

#endif
