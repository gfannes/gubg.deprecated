#ifndef HEADER_gubg_math_tau_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_tau_hpp_ALREADY_INCLUDED

#include <cmath>

namespace gubg
{
    const double pi = 3.14159265358979323846264338327950288419716939937510;
    namespace tau
    {
        //Variables in this namespace are assumed to be in tau coordinates:
        // * 1 tau == 2 pi == 360 degree == circumference of unit circle

        //This returns the shorted (signed) difference between a and b, measured in tau
        //Return value is in [-0.5, 0.5)
        template <typename T>
            T diff(T a, T b)
            {
                //Map a and b in the [0,1] interval
                {
                    T integralPart;
                    a = modf(a, &integralPart);
                    if (a < 0)
                        a += 1.0;
                    b = modf(b, &integralPart);
                    if (b < 0)
                        b += 1.0;
                }
                T res = b-a;
                if (res >= 0.0)
                    return (res < 0.5) ? res : res - 1.0;
                else
                    return (res > -0.5) ? res : res + 1.0;
            }

        //This returns the shorted (unsigned) distance between a and b, measured in tau
        //Return value is in [0.0, 0.5]
        template <typename T>
            T distance(T a, T b)
            {
                return abs(gubg::tau::diff(a, b));
            }
    }
}

#endif
