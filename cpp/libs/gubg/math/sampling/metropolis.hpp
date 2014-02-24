#ifndef HEADER_gubg_math_sampling_metropolis_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_sampling_metropolis_hpp_ALREADY_INCLUDED

#include "distribution.hpp"
#include "bernoulli.hpp"
#include "vector.hpp"

namespace gubg
{
    template <typename TargetD, typename ProposalD>
    class Metropolis
    {
    public:
        Metropolis(TargetD &targetD, ProposalD &proposalD, bool symmetric = false):
            mTargetD(targetD),
            mProposalD(proposalD),
            mSymmetric(symmetric),
            mBernoulli(0.0){}
    
        bool run(typename TargetD::DomainType &location, long nrSteps, bool optimize = false)
            {
                double logP0, logP1, alpha, bestLogP;
                typename TargetD::DomainType newLocation, bestLocation;
                bool b;

                bestLogP = logP0 = mTargetD.logDensity(location);
                bestLocation = location;
                mLocations.resize(nrSteps);
                for (long i=0;i<nrSteps;++i)
                {
                    // Generate a new candidate location
                    mProposalD.generate(newLocation, location);
                    logP1 = mTargetD.logDensity(newLocation);
                
                    // Compute the likelihood ratio
                    alpha = (logP1-logP0);
                    if (!mSymmetric)
                        alpha += mProposalD.logDensity(location, newLocation) - mProposalD.logDensity(newLocation, location);

                    // Accept if likelihood ratio is large enough (we increased in probability or did not decrease to much)
                    if (alpha >= 0)
                    {
                        location = newLocation;
                        logP0 = logP1;
//                    cout << "Accept" << endl;
                        if (optimize && logP0>bestLogP)
                        {
                            bestLogP = logP0;
                            bestLocation = location;
                        }
                    } else
                    {
                        mBernoulli.setProbTrue(exp(alpha));
                        mBernoulli.generate(b);
                        if (b)
                        {
                            location = newLocation;
                            logP0 = logP1;
//                        cout << "Accept but random" << endl;
                        } else
                        {
//                        cout << "Reject" << endl;
                        }
                    }
//                cout << "" << Vector::toString(location) << "" << endl;
                    mLocations[i] = location;
                }
                if (optimize)
                    location = bestLocation;
                return true;
            }
        vector<typename TargetD::DomainType> locations(){return mLocations;}
    private:
        TargetD &mTargetD;
        ProposalD &mProposalD;
        bool mSymmetric;
        BernoulliDistribution mBernoulli;
        vector<typename TargetD::DomainType> mLocations;
    };
}

#endif
