#ifndef HEADER_gubg_math_stat_random_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_random_hpp_ALREADY_INCLUDED

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

namespace gubg
{
    class Random
    {
    public:
        Random()
            {
                initialize();
            };
        static void initialize()
            {
                if (!mInitialized)
                {
                    srand(time(0)+rand());
                    mInitialized = true;
                }
            }
        static void setSeed(unsigned int seed = 0)
            {
                srand(seed);
                mInitialized = true;
            }

        static double generateUniform(){return double(rand())/RAND_MAX;}
        static double generateUniform(double low, double high){return low+(high-low)*double(rand())/RAND_MAX;}
        // high is not inclusive, low is
        static long generateUniformDiscrete(long low, long high)
            {
                return low+rand()%(high-low);
            }
        template <typename R, typename T>
        static bool generateDiscrete(R &ix, const vector<T> &probs)
            {
                T cumulProb = 0.0;
                T x = generateUniform();
                for (ix=0; x<probs.size(); ++ix)
                {
                    cumulProb += probs[ix];
                    if (cumulProb > 1.0000000001)
                        cerr << "The sum of the probs is more than 1.0 in generateDiscrete: " << cumulProb << "" << endl;
                    if (x <= cumulProb)
                        return true;
                }
                cerr << "The sum of the probs is less than 1.0 in generateDiscrete: " << x << " < " << cumulProb << "" << endl;
                return false;
            }

        //according to: Stochastic simulation, B. Ripley, section
        static double generateGaussian()
            {
                double dX1,dX2,dV,dY,dZ;
                const double dsqrt2dive = 0.85776388496071;            //sqrt(2/exp(1))
                const double depowmin1dot35 = 0.25924026064589;        //exp(-1.35)
                bool bAccepted = false;
        
                do
                {
                    dX1 = generateUniform();
                    dX2 = generateUniform();
                    dV = dsqrt2dive*(dX2+dX2-1);
                    if (dX1 > 0)
                    {
                        dZ = dV/dX1;
                        dY = dZ*dZ/4;
                        if (dY <= (1-dX1))
                        {
                            bAccepted = true;
                        } else
                        {
                            if ((dY <= depowmin1dot35/dX1 + 0.35) && (dY <= -log(dX1)))
                                bAccepted = true;                                
                        }
                    }                
                } while (!bAccepted);
                return dZ;        
            }
        static double generateGaussian(double mean,double sigma)
            {
                if (sigma == 0.0)
                    return mean;
                return sigma*generateGaussian()+mean;
            }
    private:
        static bool mInitialized;
    };
}

#endif
