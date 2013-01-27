#include "gubg/Cartouche.hpp"
#include "gubg/SmartRange.hpp"
using namespace gubg;
using namespace std;

namespace 
{
    typedef SmartRange<string> SR;
}

bool Cartouche::parse(const string &str)
{
    SR sr(string(str));
    return true;
}
