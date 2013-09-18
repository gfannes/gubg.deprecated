#ifndef HEADER_da_build_Builder_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Builder_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/Types.hpp"
#include "da/build/Sources.hpp"
#include "da/build/Headers.hpp"
#include "da/package/Packages.hpp"

namespace da
{
    class Builder
    {
        public:
            Builder();

            typedef gubg::file::File SourceFile;
            ReturnCode process(const SourceFile &);

            void extractCompileSettings(CompileSettings &cs) const {cs = compileSettings_;}
            void extractLinkSettings(LinkSettings &ls) const {packages_.extractLinkSettings(ls);}
			ReturnCode addTree(const gubg::file::File &);
			ReturnCode addInclude(const gubg::file::File &);

            const Sources &sources() const {return sources_;}
            Headers headers(Source::Ptr) const;
            const IncludePaths &includePaths() const {return compileSettings_.includePaths;}

        private:
            da::package::Packages packages_;
			da::package::Forest::Ptr forest_;
            Sources sources_;
            Headers headers_;
            typedef std::map<Source::Ptr, Headers> HeadersPerSource;
            HeadersPerSource headersPerSource_;
            CompileSettings compileSettings_;
    };
}

#endif
