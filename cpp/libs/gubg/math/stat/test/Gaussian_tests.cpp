#include "gubg/math/stat/Gaussian.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    typedef gubg::Gaussian<double> Gaussian;
}
int main()
{
    Gaussian g;
    return 0;
}
#include "gubg/log/end.hpp"
