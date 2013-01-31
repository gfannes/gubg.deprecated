#ifndef HEADER_da_package_SDL_hpp_ALREADY_INCLUDED
#define HEADER_da_package_SDL_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    namespace package
    {
        class SDL: public Package
        {
            public:
                template <typename T>
                static Ptr create(T t){return Ptr(new SDL(t));}

                //Package API
                virtual std::string name() const {return "sdl";}
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

            private:
                SDL(const gubg::file::File &base);

                gubg::file::File base_;
                gubg::file::File libDir_;
        };
    }
}

#endif
