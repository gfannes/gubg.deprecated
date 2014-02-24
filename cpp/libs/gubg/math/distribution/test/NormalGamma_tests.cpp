#include "gubg/math/distribution/NormalGamma.hpp"
using namespace gubg::math::distribution;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    S();
    NormalGamma<double> ng(42.0, 1, 2, 3);
    double mean, precision;
    ng.mean(mean, precision);
    L(STREAM(mean, precision));
    return 0;
}
#include "gubg/log/end.hpp"
