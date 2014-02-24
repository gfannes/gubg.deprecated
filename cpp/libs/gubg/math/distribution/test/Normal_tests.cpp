#include "gubg/math/distribution/Normal.hpp"
using namespace gubg::math::distribution;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    S();
    Normal<double> n;
    double mean;
    n.mean(mean);
    L(STREAM(mean));
    double stddev;
    n.standardDeviation(stddev);
    L(STREAM(stddev));
    return 0;
}
#include "gubg/log/end.hpp"
