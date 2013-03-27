#define GUBG_LOG
#include "gubg/Testing.hpp"
#include "gubg/math/stat/Mixture.hpp"
#include "gubg/math/stat/Gaussian.hpp"

typedef gubg::Mixture<gubg::GaussianD<>> Mixture;

int main()
{
    Mixture mixture(3);
    Mixture::value_type v;
    mixture.draw(v);
    LOG_S(main, gubg::testing::toString_(v));
    return 0;
}
