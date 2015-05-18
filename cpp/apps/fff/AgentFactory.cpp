#include "fff/AgentFactory.hpp"
#include "fff/agents/Starter.hpp"
#include "fff/agents/ExtractIncludes.hpp"
#include "fff/agents/FixIncludeGuard.hpp"
#include "fff/agents/ParseInclude.hpp"
#include "fff/agents/ResolveHeader.hpp"
#include "fff/agents/Hasher.hpp"
#include "fff/agents/Compiler.hpp"
#include "fff/agents/Linker.hpp"
#include "fff/agents/Runner.hpp"
#include "fff/agents/Search.hpp"
#include "fff/agents/Chai.hpp"
using namespace std;

namespace fff { 

    Agent_itf::Ptr AgentFactory::createAgent(const string &name)
    {
#define L_ELSE_IF(type) else if (name == #type) return Agent_itf::Ptr(new agents::type())
        if (false) {}
        L_ELSE_IF(Starter);
        L_ELSE_IF(ExtractIncludes);
        L_ELSE_IF(FixIncludeGuard);
        L_ELSE_IF(ParseInclude);
        L_ELSE_IF(ResolveHeader);
        L_ELSE_IF(Hasher);
        L_ELSE_IF(Compiler);
        L_ELSE_IF(Linker);
        L_ELSE_IF(Runner);
        L_ELSE_IF(Search);
        return Agent_itf::Ptr();
    }

    Agent_itf::Ptr AgentFactory::createChai(const gubg::file::File &script)
    {
        auto chai = std::make_shared<agents::Chai>(script);
        if (!chai->ok())
            chai.reset();
        return chai;
    }

}
