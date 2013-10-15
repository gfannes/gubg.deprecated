#include "gubg/Testing.hpp"
#include "gubg/tmp/Inheritance.hpp"
#include <string>
using namespace gubg::tmp;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
struct Base1 {};
struct Base2 {};
struct Derived1: Base1 {};
struct Derived2: Base2 {};
struct Derived12: Base1, Base2 {};
struct Derived_1: Derived1 {};
struct Unrelated {};

int main()
{
    TEST_TAG(Inheritance);
    TEST_TRUE((Inheritance<Derived1, Base1>::Value));
    TEST_FALSE((Inheritance<Base1, Derived1>::Value));
    TEST_TRUE((Inheritance<Derived12, Base1>::Value));
    TEST_FALSE((Inheritance<Base1, Derived12>::Value));
    TEST_TRUE((Inheritance<Derived12, Base2>::Value));
    TEST_FALSE((Inheritance<Base2, Derived12>::Value));
    TEST_TRUE((Inheritance<Derived_1, Base1>::Value));
    TEST_FALSE((Inheritance<Base1, Derived_1>::Value));
    TEST_FALSE((Inheritance<Unrelated, Base1>::Value));
    TEST_TRUE((Inheritance<Base1, Base1>::Value));
    TEST_TRUE((Inheritance<Derived_1, Derived_1>::Value));
    return 0;
}
#include "gubg/log/end.hpp"
