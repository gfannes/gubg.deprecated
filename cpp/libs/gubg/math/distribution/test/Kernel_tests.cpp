#include "gubg/math/distribution/Kernel.hpp"
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    MSS_BEGIN(int, main);

    typedef gubg::math::distribution::Kernel<double, double> Kernel;
    Kernel kernel;
    vector<double> data({1.0, 2.0, 2.5});
    kernel.learn(data);
    L(kernel.density(0.0));
    L(kernel.density(1.0));

    MSS_END();
}
#include "gubg/log/end.hpp"
