#include "parsing/Component.hpp"
#include "parsing/Token.hpp"
#include <sstream>
using namespace meta;
using namespace std;

string TokenComposite::toString() const
{
    ostringstream oss;
    for (auto &v: childs_)
        oss << toCode(v->range_);
    return oss.str();
}

#ifdef UnitTest
int main()
{
    return 0;
}
#endif
