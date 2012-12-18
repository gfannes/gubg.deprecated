#ifndef HEADER_da_build_Source_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Source_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/build/Headers.hpp"
#include "gubg/file/Forest.hpp"
#include "gubg/file/File.hpp"
#include "gubg/Verbose.hpp"
#include <memory>

namespace da
{
    class Source: gubg::Verbose<false>
    {
        public:
            typedef gubg::file::File File;

            typedef std::shared_ptr<Source> Ptr;
            static Ptr create(File);

            const File &file() const {return file_;}

            //Returns all headers accessed (recursively) from this source file which could be found in the forest
            ReturnCode searchForHeaders(Headers &, const gubg::file::Forest &);

        private:
            Source(File);

            const File file_;
    };
}

#endif
