#ifndef HEADER_gubg_file_basic_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_basic_hpp_ALREADY_INCLUDED

#include "gubg/file/Codes.hpp"
#include <string>

namespace gubg
{
    namespace file
    {
        namespace basic
        {
            using namespace std;
            enum class ReturnCode
            {
                MSS_DEFAULT_CODES,
                InvalidPath, InvalidCurrentWorkingDirectory, InvalidHomeDirectory,
                PathIsEmpty, PartsIsEmpty, PathIsNotAbsolute,
                CouldNotOpenDir, CouldNotReadEntry,
            };
            ReturnCode expandPath(string &pathE, const string &path);

            typedef vector<string> Files;
            typedef vector<string> Directories;
            ReturnCode getDirectoryContent(Directories &, Files &, const string &path);
        }
    }
}

#endif
