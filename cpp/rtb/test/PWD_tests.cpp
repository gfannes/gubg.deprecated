#include "rtb/PWD.hpp"
#include "gubg/Array.hpp"
#include "gubg/Testing.hpp"

namespace 
{
    double angle = 1.0;
    typedef rtb::pwd::State<3, angle> PWD;
}

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    PWD pwd;
    pwd.setDistances(gubg::make_array(1, 3, 2));
    pwd.setAngles(gubg::make_array(1, 3, 2));
    return 0;
}
#include "gubg/log/end.hpp"
