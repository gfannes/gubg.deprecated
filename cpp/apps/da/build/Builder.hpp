#ifndef HEADER_da_build_Builder_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Builder_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/build/Configuration.hpp"
#include "da/build/Sources.hpp"
#include "da/build/Headers.hpp"

namespace da
{
    class Builder
    {
        public:
            Builder(const Configuration &configuration):
                configuration_(configuration){}

            typedef gubg::file::File SourceFile;
            ReturnCode process(const SourceFile &);

            const Sources &sources() const {return sources_;}

        private:
            const Configuration configuration_;
            Sources sources_;
            Headers headers_;
    };
}

#endif
