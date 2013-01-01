#ifndef HEADER_da_build_Builder_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Builder_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/Types.hpp"
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
            Headers headers(Source::Ptr) const;
            const IncludePaths &includePaths() const {return includePaths_;}

        private:
            const Configuration configuration_;
            Sources sources_;
            Headers headers_;
            typedef std::map<Source::Ptr, Headers> HeadersPerSource;
            HeadersPerSource headersPerSource_;
            IncludePaths includePaths_;
    };
}

#endif
