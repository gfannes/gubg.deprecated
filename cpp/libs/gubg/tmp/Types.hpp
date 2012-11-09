#ifndef HEADER_gubg_tmp_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_Types_hpp_ALREADY_INCLUDED

namespace gubg
{
    namespace tmp
    {
        struct Null {};
        struct NotFound {};
        struct True {static const bool Value = true;};
        struct False {static const bool Value = false;};

        typedef char Small;
        class Large {char chs[2];};
    }
}

#endif
