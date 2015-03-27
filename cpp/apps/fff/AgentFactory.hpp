#ifndef HEADER_fff_AgentFactory_hpp_ALREADY_INCLUDED
#define HEADER_fff_AgentFactory_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"
#include <string>

namespace fff { 
    class AgentFactory
    {
        public:
            //AgentChain createAgentChain(const std::string &name);
            Agent_itf::Ptr createAgent(const std::string &name);
    };
} 

#endif
