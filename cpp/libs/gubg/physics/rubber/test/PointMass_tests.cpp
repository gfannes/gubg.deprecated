#include "gubg/physics/Rubber.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::rubber;

int main()
{
    TEST_TAG(PointMass);
    PointMass<double, 2> pm;
    TEST_EQ(0.0, pm.mass);
    return 0;
}
