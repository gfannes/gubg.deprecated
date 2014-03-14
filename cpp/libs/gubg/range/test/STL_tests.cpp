#include "gubg/range/STL.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::range;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    string str;
    {
        Traits<string>::Range r(str);
        char &ch = r.front();
    }
    {
        Traits<const string>::Range r(str);
        const char &ch = r.front();
    }
    return 0;
}
#include "gubg/log/end.hpp"
