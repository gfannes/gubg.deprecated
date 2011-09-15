#include "gubg/Exception.hpp"

std::ostream &operator<<(std::ostream &os, const gubg::Exception &exc)
{
    return os << exc.what();
}

#ifdef UnitTest
#include <iostream>
using namespace std;
struct TestException: gubg::Exception
{
    TestException():
        gubg::Exception("Test exception"){cout << "ctor " << this << endl;}
    TestException(const TestException &rhs):
        gubg::Exception("Copied test exception")
    {
        cout << "copy ctor " << &rhs << " to " << this << endl;
    }
    //If you don't provide a move ctor, the copy ctor will be used
    TestException(TestException &&rhs):
        gubg::Exception("Moved test exception")
    {
        cout << "move ctor " << &rhs << " to " << this << endl;
    }
    virtual ~TestException() throw() {cout << "dtor " << this << endl;}
};
int main()
{
    try
    {
        cout << "Before throwing" << endl;
        gubg::Exception::raise(TestException());
        cout << "After throwing" << endl;
    }
    catch (gubg::Exception &exc)
    {
        cout << "Caught exception: " << exc << endl;
    }
}
#endif
