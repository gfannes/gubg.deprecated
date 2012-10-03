#ifndef gubg_tmp_Types_hpp
#define gubg_tmp_Types_hpp

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
