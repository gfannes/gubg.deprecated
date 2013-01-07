#ifndef HEADER_da_build_Source_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Source_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/Types.hpp"
#include "da/build/Headers.hpp"
#include "da/package/Packages.hpp"
#include "gubg/file/File.hpp"
#include "gubg/Verbose.hpp"
#include <memory>
#include <vector>

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
            //and the corresponding include paths
            ReturnCode searchForHeaders(Headers &, IncludePaths &, SourceFiles &sisterFiles, const package::Packages &);

        private:
            Source(File);

            const File file_;
    };
}

#endif
