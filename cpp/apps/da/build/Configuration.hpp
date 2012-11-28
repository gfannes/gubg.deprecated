#ifndef HEADER_da_build_Configuration_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Configuration_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include <vector>
#include <string>

namespace da
{
    class Configuration
    {
        public:
            struct Compiler
            {
                std::vector<std::string> defines;
                std::vector<std::string> settings;
                std::vector<gubg::file::File> includePaths;
            };
            Compiler compiler;

            Configuration();

        private:
    };
}

#endif
