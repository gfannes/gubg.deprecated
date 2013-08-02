#ifndef HEADER_da_package_Packages_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Packages_hpp_ALREADY_INCLUDED

#include "da/Types.hpp"
#include "da/package/Package.hpp"
#include "da/package/Local.hpp"
#include "da/package/GUBG.hpp"
#include "da/package/Boost.hpp"
#include "da/package/Nana.hpp"
#include "da/package/Arduino.hpp"
#include "da/package/SDL.hpp"
#include "da/package/DecodeIt.hpp"
#include "da/package/Libs.hpp"
#include "da/package/Forest.hpp"
#include "gubg/file/File.hpp"
#include <list>

namespace da
{
    namespace package
    {
        class Packages
        {
            public:
                typedef gubg::file::File File;

                //Adding packages
                template <typename Pkg>
                Packages &operator<<(const Pkg &pkg)
                {
                    packages_.push_back(pkg);
                    return *this;
                }

                //Removes unexisting packages, and selects the first one if duplicates are present
                void prune();

                ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);
                
                void extractCompileSettings(CompileSettings &cs) const;
                void extractLinkSettings(LinkSettings &ls) const;

                std::vector<std::string> names() const;

                size_t size() const {return packages_.size();}

            private:
                typedef std::list<Package::Ptr> Packages_;
                Packages_ packages_;
        };
    }
}

#endif
