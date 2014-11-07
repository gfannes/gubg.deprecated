#include "fff/ToolFactory.hpp"
#include "fff/tools/Starter.hpp"
#include "fff/tools/ParseIncludes.hpp"
#include "fff/tools/ResolveHeader.hpp"
#include "fff/tools/Compiler.hpp"
#include "fff/tools/Linker.hpp"
#include "fff/tools/Runner.hpp"
using namespace std;

namespace fff { 
    ToolChain ToolFactory::createToolChain(const string &name)
    {
        ToolChain res;
        if (false) {}
        else if (name == "exe")
        {
            res.push_back(createTool("Starter"));
            res.push_back(createTool("ParseIncludes"));
            res.push_back(createTool("ResolveHeader"));
            res.push_back(createTool("Compiler"));
            res.push_back(createTool("Linker"));
        }
        return res;
    }
    Tool_itf::Ptr ToolFactory::createTool(const string &name)
    {
#define L_ELSE_IF(type) else if (name == #type) return Tool_itf::Ptr(new tools::type())
        if (false) {}
        L_ELSE_IF(Starter);
        L_ELSE_IF(ParseIncludes);
        L_ELSE_IF(ResolveHeader);
        L_ELSE_IF(Compiler);
        L_ELSE_IF(Linker);
        L_ELSE_IF(Runner);
        return Tool_itf::Ptr();
    }
} 
