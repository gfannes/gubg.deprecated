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
            typedef gubg::file::File SourceFile;
            ReturnCode process(const SourceFile &);

            void extractCompileSettings(CompileSettings &cs) const {configuration_.packages().extractCompileSettings(cs);}
            void extractLinkSettings(LinkSettings &ls) const {configuration_.packages().extractLinkSettings(ls);}

            const Sources &sources() const {return sources_;}
            Headers headers(Source::Ptr) const;
            const IncludePaths &includePaths() const {return compileSettings_.includePaths;}

        private:
            Configuration configuration_;
            Sources sources_;
            Headers headers_;
            typedef std::map<Source::Ptr, Headers> HeadersPerSource;
            HeadersPerSource headersPerSource_;
            CompileSettings compileSettings_;
    };
}

#endif
