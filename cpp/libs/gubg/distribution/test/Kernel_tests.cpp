#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/distribution/Kernel.hpp"

int main()
{
    MSS_BEGIN(int, main);
    typedef gubg::distribution::Kernel<double, double> Kernel;
    Kernel kernel;
    vector<double> data({1.0, 2.0, 2.5});
    kernel.learn(data);
    LOG_M(kernel.density(0.0));
    LOG_M(kernel.density(1.0));
    MSS_END();
}
