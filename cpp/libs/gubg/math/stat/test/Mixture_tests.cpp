#include "gubg/math/stat/Mixture.hpp"
#include "gubg/math/stat/Gaussian.hpp"
#include "gubg/Plot.hpp"
#include <thread>

namespace 
{
    typedef gubg::Gaussian<double> Gaussian;
    typedef gubg::Mixture<Gaussian> Mixture;
}

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    S();
    const size_t Nr = 10000;
    Mixture m(3);
    m.component(1).setMean(-5);
    m.component(2).setMean(2).setSigma(0.3);
    std::vector<double> vs(Nr);
    gubg::Plot p;
    for (int i = 0; i < 100; ++i)
    {
        m.draw(vs);
        p.histogram(vs, 200);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
#include "gubg/log/end.hpp"
