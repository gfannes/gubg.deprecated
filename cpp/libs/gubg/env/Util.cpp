#include "gubg/env/Util.hpp"
#include "gubg/env/Variables.hpp"
using std::string;
//We don't use std::regex, it seems to be alpha
#define L_USE_BOOST_REGEX
#ifdef L_USE_STD_REGEX
#include <regex>
using namespace std;
#endif
#ifdef L_USE_BOOST_REGEX
#include "boost/regex.hpp"
using namespace boost;
#endif

namespace 
{
    regex re("^([^\\$]*)\\$([a-zA-z_]+)");
}
string gubg::env::expand(const string &str)
{
    string res;
    auto it = str.begin();
    auto end = str.end();
    auto vars = gubg::env::Variables::shell();
    while (it != end)
    {
        smatch md;
        if (!regex_search(it, end, md, re))
        {
            res.append(it, end);
            return res;
        }
        it += md.length();

        string v;
        if (vars.get(v, md.str(2)))
        {
            if (md.length(1) > 0)
                res.append(md.str(1));
            res.append(v);
        }
        else
            res.append(md.str());
    }
    return res;
}
