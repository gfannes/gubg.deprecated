#ifndef gubg_tmp_FindBaseByName_hpp
#define gubg_tmp_FindBaseByName_hpp

#include "gubg/tmp/NameType.hpp"
#include "gubg/tmp/TypeList.hpp"
#include "gubg/tmp/If.hpp"
#include "gubg/tmp/Types.hpp"

namespace gubg
{
    namespace tmp
    {
        //Template that searches _recursively_ to the type with name WN (=WantedName)
        //It provides you with:
        // * Value: true/false => indicating if the name was found (used internally)
        // * Type: the type attached to the given name (or NotFound if not found)
        template <typename WN, typename List>
            struct FindBaseByName;
        template <typename WN, typename H, typename R>
            struct FindBaseByName<WN, TypeList<H, R> >
            {
                enum: bool {Value = (FindBaseByName<WN, R>::Value || FindBaseByName<WN, typename H::Type::Pubs>::Value)};
                typedef typename If<FindBaseByName<WN, R>::Value,
                        typename FindBaseByName<WN, R>::Type,
                        typename FindBaseByName<WN, typename H::Type::Pubs>::Type>::Type
                            Type;
            };
        template <typename WN, typename T, typename R>
            struct FindBaseByName<WN, TypeList<NameType<WN, T>, R> >
            {
                enum: bool {Value = true};
                typedef T Type;
            };
        template <typename WN>
            struct FindBaseByName<WN, Null>
            {
                enum: bool {Value = false};
                typedef NotFound Type;
            };
    }
}

#endif
