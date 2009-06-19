#include <iostream>

#include "math.hpp"

using namespace std;

namespace gubg
{
    namespace Math
    {
// Compute the a and b values for the linear function y = a*x+b that will map x1 on y1 and x2 on y2.
// Return false if x1 and x2 are the same (so not enough information is provided to compute a and b)
	bool computeLinTrans(double &a, double &b, double x1, double y1, double x2, double y2)
	{
	    if (x1 == x2)
	    {
		cerr << "Both input values are the same" << endl;
		return false;
	    }
	    a = (y2-y1)/(x2-x1);
	    b = y1 - a*x1;
	    return true;
	}
	bool linTrans(double &output, double input, double a, double b)
	{
	    output = a*input+b;
	    return true;
	}

    };
};
