#ifndef HEADER_fff_ToolFactory_hpp_ALREADY_INCLUDED
#define HEADER_fff_ToolFactory_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include <string>

namespace fff { 
    class ToolFactory
    {
        public:
            ToolChain createToolChain(const std::string &name);
            Tool_itf::Ptr createTool(const std::string &name);
    };
} 

#endif
