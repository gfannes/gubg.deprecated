#include "gubg/testing/Testing.hpp"
#include "gubg/tmp/Equal.hpp"
#include "gubg/l.hpp"
#include <string>
using namespace gubg::tmp;

struct T1 {};
struct T2 {};
typedef T1 T3;
struct T4: T1 {};

int main()
{
    TEST_TAG(Equal);
    TEST_TRUE((Equal<T1, T1>::Value));
    TEST_TRUE((Equal<T2, T2>::Value));
    TEST_TRUE((Equal<T3, T3>::Value));
    TEST_TRUE((Equal<T4, T4>::Value));
    TEST_TRUE((Equal<T1, T3>::Value));
    TEST_FALSE((Equal<T1, T2>::Value));
    TEST_FALSE((Equal<T2, T1>::Value));
    TEST_FALSE((Equal<T2, T3>::Value));
    TEST_FALSE((Equal<T3, T2>::Value));
    TEST_FALSE((Equal<T1, T4>::Value));
    TEST_FALSE((Equal<T4, T1>::Value));
    return 0;
}
