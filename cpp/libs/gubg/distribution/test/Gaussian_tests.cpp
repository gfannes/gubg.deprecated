#include "gubg/distribution/Gaussian.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::distribution;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    L(gaussian_prob(0.0, 0.001, 0.01));
    L(gaussian_prob(0.0, 0.01, 0.01));
    return 0;
}
#include "gubg/log/end.hpp"
