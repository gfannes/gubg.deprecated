#ifndef HEADER_fff_ToolFactory_hpp_ALREADY_INCLUDED
#define HEADER_fff_ToolFactory_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include <string>

namespace fff { 
    class ToolFactory
    {
        public:
            ToolChain createToolChain(const std::string &name);

        private:
            Tool_itf::Ptr create_(const std::string &name);
    };
} 

#endif
