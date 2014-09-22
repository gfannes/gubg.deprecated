#include "fff/ToolFactory.hpp"
#include "fff/Starter.hpp"
#include "fff/ParseIncludes.hpp"
#include "fff/ResolveHeader.hpp"
#include "fff/Compiler.hpp"
#include "fff/Linker.hpp"
#include "fff/Runner.hpp"
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
#define L_ELSE_IF(type) else if (name == #type) return Tool_itf::Ptr(new type())
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
