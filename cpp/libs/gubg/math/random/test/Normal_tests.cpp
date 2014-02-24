#include "gubg/math/random/Normal.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::math::random;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    L(normal_density(0.0, 0.001, 0.01));
    L(normal_density(0.0, 0.01, 0.01));
    return 0;
}
#include "gubg/log/end.hpp"
