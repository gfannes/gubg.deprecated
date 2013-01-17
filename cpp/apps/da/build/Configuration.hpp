#ifndef HEADER_da_build_Configuration_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Configuration_hpp_ALREADY_INCLUDED

#include "da/package/Packages.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Forest.hpp"
#include <vector>
#include <string>

namespace da
{
    class Configuration
    {
        public:
            bool verbose;

            struct Compiler
            {
                Defines defines;
                std::vector<std::string> settings;
                IncludePaths includePaths;
            };
            Compiler compiler;

            struct Linker
            {
                std::vector<std::string> settings;
                Libraries libraries;
                LibraryPaths libraryPaths;
            };
            Linker linker;

            Configuration();

            const gubg::file::Forest &forest() const {return forest_;}
            const da::package::Packages &packages() const {return packages_;}

        private:
            gubg::file::Forest forest_;
            da::package::Packages packages_;
    };
}

#endif
