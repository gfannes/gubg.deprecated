#ifndef HEADER_gubg_tuple_TypeTransform_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tuple_TypeTransform_hpp_ALREADY_INCLUDED

#include <tuple>

namespace gubg { namespace tuple { 

    namespace details { 
        template <size_t Nr, typename Tuple, template <typename, size_t> class Transform>
            struct TypeTransform
            {
                //Create the transformed tuple type upto Nr-1
                typedef typename TypeTransform<Nr-1, Tuple, Transform>::Type PartialTransTuple;

                //Create the last transformed tuple type as a one-element tuple
                typedef typename std::tuple_element<Nr-1, Tuple>::type LastTupleType;
                typedef std::tuple<typename Transform<LastTupleType, Nr-1>::Type> LastTransTupleType;

                //Concat both types
                typedef decltype(std::tuple_cat(PartialTransTuple(), LastTransTupleType())) Type;
            };

        //std::tuple supports empty type list
        template <typename Tuple, template <typename, size_t> class Transform>
            struct TypeTransform<0, Tuple, Transform>
            {
                typedef std::tuple<> Type;
            };

        template <typename Tuple, template <typename, size_t> class Transform>
            struct TypeTransform<1, Tuple, Transform>
            {
                typedef typename std::tuple_element<0, Tuple>::type T;
                typedef std::tuple<typename Transform<T, 0>::Type> Type;
            };
    } 

    template <typename Tuple, template <typename, size_t> class Transform>
        struct TypeTransform
        {
            typedef typename details::TypeTransform<std::tuple_size<Tuple>::value, Tuple, Transform>::Type Type;
        };


} } 

#endif
