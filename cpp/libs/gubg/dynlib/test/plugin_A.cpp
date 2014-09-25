#include "A.hpp"
#include <iostream>
using namespace std;
class A: public A_itf
{
    public:
        A()
        {
            cout << "A::ctor" << endl;
        }
        void foo() override
        {
            cout << "A::foo" << endl;
        }
    private:
};

#include "gubg/dynlib/Plugin.hpp"
GUBG_DYNLIB_SHARE(A);
