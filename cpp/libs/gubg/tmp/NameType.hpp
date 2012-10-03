#ifndef gubg_tmp_NameType_hpp
#define gubg_tmp_NameType_hpp

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
