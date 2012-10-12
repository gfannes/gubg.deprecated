#ifndef gubg_file_Codes_hpp
#define gubg_file_Codes_hpp

#include "gubg/mss.hpp"

namespace gubg
{
    namespace file
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            ExpectedRegular,
            ExpectedDirectory,
            CouldNotOpenFile,
            CouldNotOpenDir,
            CouldNotReadEntry,
        };
    }
}

#endif
