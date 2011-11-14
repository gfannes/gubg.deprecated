#ifndef gubg_file_basic_hpp
#define gubg_file_basic_hpp

#include "gubg/mss.hpp"
#include <string>

namespace gubg
{
    namespace file
    {
        namespace basic
        {
            enum class ReturnCode
            {
                MSS_DEFAULT_CODES,
                InvalidPath, InvalidCurrentWorkingDirectory, InvalidHomeDirectory,
                PathIsEmpty, PartsIsEmpty, PathIsNotAbsolute,
            };
            ReturnCode expandPath(std::string &pathE, const std::string &path);
        }
    }
}

#endif
