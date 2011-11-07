#include "gubg/parse/basic.hpp"
using namespace std;

bool gubg::parse::startsWith(const string &haystack, const string &needle)
{
    return needle.size() <= haystack.size() && needle == haystack.substr(0, needle.size());
}

vector<string> gubg::parse::tokenize(const string &str, const string &token)
{
    vector<string> res;
    size_t  prevIX = 0;
    size_t ix = str.find(token);
    const size_t tokenSize = token.size();
    while (ix != string::npos)
    {
        res.push_back(str.substr(prevIX, ix - prevIX));
        prevIX = ix + tokenSize;
        ix = str.find(token, prevIX);
    }
    res.push_back(str.substr(prevIX));
    return res;
}
