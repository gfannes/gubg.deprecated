#include "testing/TestMaster.hpp"
#include <mutex>
using namespace gubg;
using namespace std;

namespace
{
    TestMaster *testMaster = nullptr;
    mutex testMasterMutex;
}

TestMaster &TestMaster::instance()
{
    lock_guard<mutex> lock(testMasterMutex);
    if (nullptr == testMaster)
        testMaster = new TestMaster;
    return *testMaster;
}

#ifdef UnitTest
int main()
{
    TestMaster::instance();
}
#endif
