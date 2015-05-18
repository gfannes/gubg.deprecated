#ifndef HEADER_fff_AgentFactory_hpp_ALREADY_INCLUDED
#define HEADER_fff_AgentFactory_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Agent.hpp"
#include "gubg/file/File.hpp"
#include <string>

namespace fff { 
    class AgentFactory
    {
        public:
            gubg::bbs::Agent_itf::Ptr createAgent(const std::string &name);
            gubg::bbs::Agent_itf::Ptr createChai(const gubg::file::File &script);
    };
} 

#endif
