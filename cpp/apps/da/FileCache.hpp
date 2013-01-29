#ifndef aoeu
#define aoeu

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
