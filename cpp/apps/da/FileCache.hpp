#ifndef HEADER_da_FileCache_hpp_ALREADY_INCLUDED
#define HEADER_da_FileCache_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"

namespace da
{
    class FileCache
    {
        public:
            typedef gubg::file::File File;

            FileCache();

            const File &dir() const {return dir_;}

        private:
            File dir_;
    };
}

#endif
