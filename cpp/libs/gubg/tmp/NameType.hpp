#ifndef HEADER_gubg_tmp_NameType_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_NameType_hpp_ALREADY_INCLUDED

namespace gubg
{
    namespace tmp
    {
        //Template that creates a Name-Type pair
        template <typename NameT, typename TypeT>
            struct NameType
            {
                typedef NameT Name;
                typedef TypeT Type;
            };
    }
}

#endif
