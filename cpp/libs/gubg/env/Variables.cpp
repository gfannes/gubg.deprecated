#include "gubg/env/Variables.hpp"
#include "gubg/l.hpp"
#include <unistd.h>
using namespace gubg::env;
using namespace std;

extern char **environ;

Variables Variables::shell()
{
    Variables variables;
    for (auto env = ::environ; *env; ++env)
    {
        const string str(*env);
        auto ix = str.find('=');
        if (ix == string::npos)
            //Should not happen, the environment is not OK, it should contain an equality sign
            continue;
        variables.valuePerVariable_[str.substr(0, ix)] = str.substr(ix+1);
    }
    return variables;
}

bool Variables::get(Value &value, const Variable &variable) const
{
    auto it = valuePerVariable_.find(variable);
    if (it == valuePerVariable_.end())
        return false;
    value = it->second;
    return true;
}
