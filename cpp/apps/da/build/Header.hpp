#ifndef HEADER_da_build_Header_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Header_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include <memory>

namespace da
{
    class Header
    {
        public:
            typedef gubg::file::File File;

            typedef std::shared_ptr<Header> Ptr;
            static Ptr create(File);

            const File &file() const {return file_;}

        private:
            const File file_;
    };
}

#endif
