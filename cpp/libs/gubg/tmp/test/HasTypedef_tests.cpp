#include "gubg/testing/Testing.hpp"
#include "gubg/tmp/HasTypedef.hpp"
#include "gubg/l.hpp"
#include <string>
using namespace gubg::tmp;

struct S1 { typedef int Name; };
struct S2 { };

int main()
{
    TEST_TAG(HasTypedef);
    TEST_TRUE((HasTypedef<S1>::Value));
    TEST_FALSE((HasTypedef<S2>::Value));
    return 0;
}
