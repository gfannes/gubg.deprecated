#include "gubg/math/stat/Gaussian.hpp"
#include "gubg/Plot.hpp"
#include <thread>

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    typedef gubg::Gaussian<double> Gaussian;
}
int main()
{
    S();
    const size_t Nr = 10000;
    Gaussian g;
    Gaussian::value_type v;
    for (int i = 0; i < Nr; ++i)
        g.draw(v);
    L(v);
    std::vector<double> vs(Nr);
    gubg::Plot p;
    for (int i = 0; i < 100; ++i)
    {
        g.draw(vs);
        p.histogram(vs, 200);
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
#include "gubg/log/end.hpp"
