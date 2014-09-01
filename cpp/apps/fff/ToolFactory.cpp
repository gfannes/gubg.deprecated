#include "fff/ToolFactory.hpp"
#include "fff/ParseIncludes.hpp"
#include "fff/ResolveHeader.hpp"
using namespace std;

namespace fff { 
    ToolChain ToolFactory::createToolChain(const string &name)
    {
        ToolChain res;
        if (false) {}
        else if (name == "exe")
        {
            res.push_back(create_("ParseIncludes"));
            res.push_back(create_("ResolveHeader"));
        }
        return res;
    }
    Tool_itf::Ptr ToolFactory::create_(const string &name)
    {
#define L_ELSE_IF(type) else if (name == #type) return Tool_itf::Ptr(new type())
        if (false) {}
        L_ELSE_IF(ParseIncludes);
        L_ELSE_IF(ResolveHeader);
        return Tool_itf::Ptr();
    }
} 
