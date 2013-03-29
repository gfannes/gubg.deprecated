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
    Gaussian g;
    Gaussian::value_type v;
    for (int i = 0; i < 100000; ++i)
        g.draw(v);
    L(v);
    return 0;
}
#include "gubg/log/end.hpp"
