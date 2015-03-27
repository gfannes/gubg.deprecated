#include "fff/AgentFactory.hpp"
#include "fff/agents/Starter.hpp"
#include "fff/agents/ParseIncludes.hpp"
#include "fff/agents/ResolveHeader.hpp"
#include "fff/agents/Compiler.hpp"
#include "fff/agents/Linker.hpp"
#include "fff/agents/Runner.hpp"
#include "fff/agents/Search.hpp"
using namespace std;

namespace fff { 
#if 0
    AgentChain AgentFactory::createAgentChain(const string &name)
    {
        AgentChain res;
        if (false) {}
        else if (name == "exe")
        {
            res.push_back(createAgent("Starter"));
            res.push_back(createAgent("ParseIncludes"));
            res.push_back(createAgent("ResolveHeader"));
            res.push_back(createAgent("Compiler"));
            res.push_back(createAgent("Linker"));
        }
        return res;
    }
#endif
    Agent_itf::Ptr AgentFactory::createAgent(const string &name)
    {
#define L_ELSE_IF(type) else if (name == #type) return Agent_itf::Ptr(new agents::type())
        if (false) {}
        L_ELSE_IF(Starter);
        L_ELSE_IF(ParseIncludes);
        L_ELSE_IF(ResolveHeader);
        L_ELSE_IF(Compiler);
        L_ELSE_IF(Linker);
        L_ELSE_IF(Runner);
        L_ELSE_IF(Search);
        return Agent_itf::Ptr();
    }
}
