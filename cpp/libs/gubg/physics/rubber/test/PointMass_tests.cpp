#include "gubg/physics/Rubber.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::rubber;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(PointMass);
    PointMass<double, 2> pm;
    TEST_EQ(0.0, pm.mass);
    return 0;
}
#include "gubg/log/end.hpp"
