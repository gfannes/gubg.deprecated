#ifndef HEADER_da_build_Builder_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Builder_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/build/Configuration.hpp"

namespace da
{
    class Builder
    {
        public:
            Builder(const Configuration &configuration):
                configuration_(configuration){}

            typedef gubg::file::File SourceFile;
            ReturnCode process(const SourceFile &);

            typedef std::vector<SourceFile> SourceFiles;
            const SourceFiles &sources() const;

        private:
            const Configuration configuration_;
            SourceFiles sourceFiles_;
    };
}

#endif
