#include "gubg/math/stat/Gaussian.hpp"

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    typedef gubg::Gaussian<double> Gaussian;
}
int main()
{
    S();
    const size_t Nr = 1000000;
    Gaussian g;
    Gaussian::value_type v;
    for (int i = 0; i < Nr; ++i)
        g.draw(v);
    L(v);
    std::vector<double> vs(Nr);
    g.draw(vs);
    return 0;
}
#include "gubg/log/end.hpp"
