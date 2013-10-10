#ifndef HEADER_gubg_file_raw_file_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_raw_file_hpp_ALREADY_INCLUDED

#include "gubg/tree/raw.hpp"
#include <string>

namespace gubg
{
    namespace file
    {
        namespace raw
        {
            using namespace std;
            using namespace gubg::tree::raw;

            enum class ReturnCode
            {
                MSS_DEFAULT_CODES,
                CouldNotGetDirContent, CouldNotAddEntry, UnknownStrategy, EmptyEntry,
            };

            //The data we keep for a file or directory node
            struct Data
            {
                ~Data(){}
                string name;
            };

            enum ExpandStrategy {Shallow, Recursive};
            enum HiddenStrategy {NoHiddenFiles, IncludeHiddenFiles};

            struct Directory: Node<Data>
            {
                string path() const;
                ReturnCode expand(ExpandStrategy, HiddenStrategy);
            };
            struct Regular: Node<Data>
            {
                string filename() const;
            };
        }
    }
}

#endif
