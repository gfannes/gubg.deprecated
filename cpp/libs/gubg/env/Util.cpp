#include "gubg/env/Util.hpp"
#include "gubg/env/Variables.hpp"
using std::string;

//I tried to use regex here, but the std::regex was still alpha at that time for g++...

namespace 
{
    bool isEnvVarChar_(char ch)
    {
        if (ch == '_')
            return true;
        if ('a' <= ch && ch <= 'z')
            return true;
        if ('A' <= ch && ch <= 'Z')
            return true;
        return false;
    }
}

#define GUBG_MODULE "env"
#include "gubg/log/begin.hpp"
bool gubg::env::expand(string &res, const string &str)
{
    S();L(STREAM(str));
    res.clear();
    auto it = str.begin();
    auto end = str.end();
    auto vars = gubg::env::Variables::shell();
    while (it != end)
    {
        S();L(STREAM(res));
        
        if (*it != '$')
        {
            res.push_back(*it++);
            continue;
        }

        //Proceed beyond the $
        ++it;

        //Get the complete env var name
        string name;
        while (it != end && isEnvVarChar_(*it))
            name.push_back(*it++);

        //Get the value of this env var, if any
        string value;
        if (!vars.get(value, name))
            //Could not get env var
            return false;

        //We found the value, we append it
        res.append(value);
    }
    return true;
}
#include "gubg/log/end.hpp"
