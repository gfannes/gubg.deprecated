#ifndef HEADER_gubg_math_stat_bernoulli_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_bernoulli_hpp_ALREADY_INCLUDED

#include <vector>
#include <cmath>

#include "distribution.hpp"
#include "random.hpp"

using namespace std;

namespace gubg
{
    class BernoulliDistribution: public Distribution<bool>
    {
    public:
        BernoulliDistribution(double probTrue):
            mProbTrue(probTrue){};

        double density(bool value)
            {
                return (value ? mProbTrue : 1-mProbTrue);
            }
        double logDensity(bool value)
            {
                return log(density(value));
            }
        void setProbTrue(double probTrue){mProbTrue = probTrue;};
        bool generate(bool &b)
            {
                b = ((Random::generateUniform() <= mProbTrue) ? true : false);
                return true;
            }
    private:
        double mProbTrue;
    };
}

#endif
