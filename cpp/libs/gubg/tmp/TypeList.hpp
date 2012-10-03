#ifndef gubg_tmp_TypeList_hpp
#define gubg_tmp_TypeList_hpp

#include "gubg/tmp/Types.hpp"
#include "gubg/macro.hpp"

namespace gubg
{
    namespace tmp
    {
        //Use GUBG_TYPE_LIST(...) to create a typelist conveniently
        template <typename TypeT, typename RestT = Null>
            struct TypeList
            {
                typedef TypeT Type;
                typedef RestT Rest;
            };

        template <typename TL>
            struct TypeListSize;
        template <typename T, typename R>
            struct TypeListSize<TypeList<T, R> >
            {
                enum: size_t {Value = TypeListSize<R>::Value + 1};
                
            };
        template <>
            struct TypeListSize<Null>
            {
                enum: size_t {Value = 0};
            };
    }
}

//GUBG_TYPE_LIST(...) macro
#define GUBG_TYPE_LIST_1(_1) gubg::tmp::TypeList<_1>
#define GUBG_TYPE_LIST_2(_1,_2) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_1(_2)>
#define GUBG_TYPE_LIST_3(_1,_2,_3) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_2(_2,_3)>
#define GUBG_TYPE_LIST_4(_1,_2,_3,_4) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_3(_2,_3,_4)>
#define GUBG_TYPE_LIST_5(_1,_2,_3,_4,_5) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_4(_2,_3,_4,_5)>
#define GUBG_TYPE_LIST_6(_1,_2,_3,_4,_5,_6) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_5(_2,_3,_4,_5,_6)>
#define GUBG_TYPE_LIST_7(_1,_2,_3,_4,_5,_6,_7) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_6(_2,_3,_4,_5,_6,_7)>
#define GUBG_TYPE_LIST_8(_1,_2,_3,_4,_5,_6,_7,_8) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_7(_2,_3,_4,_5,_6,_7,_8)>
#define GUBG_TYPE_LIST_9(_1,_2,_3,_4,_5,_6,_7,_8,_9) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_8(_2,_3,_4,_5,_6,_7,_8,_9)>
#define GUBG_TYPE_LIST_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) gubg::tmp::TypeList<_1, GUBG_TYPE_LIST_9(_2,_3,_4,_5,_6,_7,_8,_9,_10)>
#define GUBG_TYPE_LIST_MACRO_CHOOSER(...) GUBG_GET_11TH_ARG(__VA_ARGS__, GUBG_TYPE_LIST_10,GUBG_TYPE_LIST_9,GUBG_TYPE_LIST_8,GUBG_TYPE_LIST_7,GUBG_TYPE_LIST_6,GUBG_TYPE_LIST_5,GUBG_TYPE_LIST_4,GUBG_TYPE_LIST_3,GUBG_TYPE_LIST_2,GUBG_TYPE_LIST_1)
#define GUBG_TYPE_LIST(...) GUBG_TYPE_LIST_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#endif
