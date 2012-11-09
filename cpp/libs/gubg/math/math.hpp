#ifndef HEADER_gubg_math_math_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_math_hpp_ALREADY_INCLUDED

#include <iostream>

using namespace std;

namespace gubg
{
    namespace Math
    {
        const double PI=3.141592653589793238462643383279502884197169399375;
    
        // Compute the a and b values for the linear function y = a*x+b that will map x1 on y1 and x2 on y2.
        // Return false if x1 and x2 are the same (so not enough information is provided to compute a and b)
        bool computeLinTrans(double &a, double &b, double x1, double y1, double x2, double y2);
        bool linTrans(double &output, double input, double a, double b);

        // Linear interpolation where the 2 input values are 0 and 1
        class UnitSegmentInterpolation
        {
        public:
            UnitSegmentInterpolation():
                mA(0),mB(0){};
            UnitSegmentInterpolation(double y0, double y1){set(y0,y1);}
            bool set(double y0, double y1)
                {
                    mB = y0;
                    mA = y1 - y0;
                    return true;
                }
            double output(double x){return mA*x + mB;}
        private:
            double mA;
            double mB;
        };

        // Quadratic interpolation where the 4 input couples are the corners of the unit square
        class UnitSquareInterpolation
        {
        public:
            UnitSquareInterpolation():
                mA(0),mB(0),mC(0),mD(0){};
            UnitSquareInterpolation(double y00,double y10, double y01, double y11){set(y00,y10,y01,y11);}
            bool set(double y00,double y10, double y01, double y11)
                {
                    mD = y00;
                    mB = y10 - y00;
                    mC = y01 - y00;
                    mA = y11 - mB - y01;
                    return true;
                }
            double output(double x, double y){return mA*x*y + mB*x + mC*y + mD;}
            void print()
                {
                    cout << "a = " << mA << ", b = " << mB << ", c = " << mC << ", d = " << mD << "" << endl;
                }
        private:
            // The output of this function is a*x*y + b*x + c*y + d
            double mA;
            double mB;
            double mC;
            double mD;
        };

    };
};
#endif
