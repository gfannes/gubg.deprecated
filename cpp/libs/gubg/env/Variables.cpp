#include "gubg/env/Variables.hpp"
#include "gubg/Platform.hpp"
#ifdef GUBG_LINUX
#include <unistd.h>
#endif
#ifdef GUBG_MINGW
#include <Windows.h>
#include <cstring>
#endif
using namespace gubg::env;
using namespace std;

#ifdef GUBG_LINUX
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
#endif
#ifdef GUBG_MINGW
Variables Variables::shell()
{
	const char *env = ::GetEnvironmentStrings();
	Variables variables;
	while (*env != '\0')
	{
		const string str(env);
		auto ix = str.find('=');
		if (ix == string::npos)
			//Should not happen, the environment is not OK, it should contain an equality sign
			continue;
		variables.valuePerVariable_[str.substr(0, ix)] = str.substr(ix+1);
		env += str.size()+1;
	}
	return variables;
}
#endif

bool Variables::get(Value &value, const Variable &variable) const
{
    auto it = valuePerVariable_.find(variable);
    if (it == valuePerVariable_.end())
        return false;
    value = it->second;
    return true;
}
