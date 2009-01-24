#ifndef bernoulli_h
#define bernoulli_h

#include <vector>
#include <cmath>

#include "distribution.hpp"
#include "random.hpp"

using namespace std;

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
    bool draw(bool &b)
        {
            b = ((Random::drawUniform() <= mProbTrue) ? true : false);
            return true;
        }
private:
    double mProbTrue;
};
#endif
