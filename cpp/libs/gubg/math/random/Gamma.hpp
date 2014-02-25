#ifndef aoeuaoeu
#define aoeuaoeu

#include "gubg/math/random/DefaultEngine.hpp"

#define GUBG_MODULE_ "random::Gamma"
#include "gubg/log/begin.hpp"
namespace gubg { namespace math { namespace random { 

	//x^(a-1)*exp(-b*x), note that beta is parametrized differently as std::gamma_distribution
	double generateGamma(double alpha, double beta)
	{
            return std::gamma_distribution<double>(alpha, 1.0/beta)(defaultEngine);
	}

} } } 
#include "gubg/log/end.hpp"

#endif
