#ifndef HEADER_gubg_bayesian_posteriorLogProb_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bayesian_posteriorLogProb_hpp_ALREADY_INCLUDED

#include <cmath>

#include "function.hpp"
#include "array.hpp"
#include "gaussian.hpp"
#include "cube.hpp"

namespace gubg
{

    template<typename ModelT, typename InputT, typename TargetT, typename PriorT = GaussianD<vector<double> > >
    class PosteriorLogProb: public Function<typename ModelT::BaseType, 0, Dimension<typename ModelT::ParameterType>::dimension>
    {
    public:
        typedef typename ModelT::ParameterType InputType;
        typedef typename ModelT::BaseType BaseType;
        typedef InputType InputDerivativeType;
        static const int InputDim = Dimension<typename ModelT::ParameterType>::dimension;

        static const double probEps = 0.0000001;
        PosteriorLogProb(ModelT &model, vector<InputT> &inputs, vector<TargetT> &targets, PriorT *prior = NULL):
            mpModel(&model),
            mpInputs(&inputs),
            mpTargets(&targets),
            mPrior(prior){}

        virtual bool clone(Function<BaseType,0,InputDim> *&res) const
            {
                return res = new PosteriorLogProb(*this);
            }

        class OutputBlock: public Array<InputT,TargetT>::EachBlock
        {
        public:
            OutputBlock(ModelT &model):
                pModel(&model){};
            bool preYield(InputT &input, TargetT &target)
                {
                    logProb=0;
                    return true;
                };
            bool yield(InputT &input, int &target)
                {
                    // if (this->index%1000==0) // do(1)
                    // cout << "Record " << this->index << "" << endl; // do(1)
                    double p = pModel->output(input);
                    if (p<0 || p>1)
                    {
                        cerr << "For binary logposterior prob calculations, I expect probability values between 0 and 1, and not " << p << endl;
                        return false;
                    } else
                    {
                        if (target==0)
                            logProb += log(1-p+probEps);
                        else
                            logProb += log(p+probEps);
                    }
                    return true;
                };
            bool yield(InputT &input, double &target)
                {
                    GaussianD<double> error(target,0.01);
                    logProb += error.logDensity(pModel->output(input));
                    return true;
                };
            double logProb;
            ModelT *pModel;
        };

        virtual bool computeOutput()
            {
                mpModel->setParameter(*this->mpInput);
                Array<InputT,TargetT> data(*mpInputs,*mpTargets);
                OutputBlock outputB(*mpModel);
                data.each(outputB);
                this->mOutput = outputB.logProb;
                if (mPrior)
                {
                    vector<double> input;
                    Cube::flatten(input, *this->mpInput);
                    this->mOutput += mPrior->logDensity(input);
                }
                return true;
            };

        // derivativeInput should get filled with the derivative wrt the inputs
        class DerivativeBlock: public Array<InputT,TargetT>::EachBlock
        {
        public:
            DerivativeBlock(ModelT &model,InputDerivativeType &derivativeInput):
                pModel(&model),
                pDerivativeInput(&derivativeInput){};
            bool preYield(InputT &input, TargetT &target)
                {
                    vector<int> path;
                    BaseType *p;
                    for (initIter(path,*pDerivativeInput) ; proceedIter(path,*pDerivativeInput,p) ;)
                        *p = 0;
                    return true;
                };
            bool yield(InputT &input, int &target)
                {
                    pModel->setInput(input);
                    double p = pModel->output();
                    if (p<0 || p>1)
                    {
                        cerr << "For binary logposterior prob calculations, I expect probability values between 0 and 1, and not " << p << endl;
                        return false;
                    } else
                    {
                        pModel->computeDerivativeParameter(tmpDerivativeInput);
                        if (target==0)
                        {
                            Cube::add(*pDerivativeInput,tmpDerivativeInput,-1.0/(1-p+probEps));
                        } else
                        {
                            Cube::add(*pDerivativeInput,tmpDerivativeInput,1.0/(p+probEps));
                        }
                    }
                    return true;
                };
            bool yield(InputT &input, double &target)
                {
                    GaussianD<double> error(target,0.01);
                    pModel->setInput(input);
                    pModel->computeDerivativeParameter(tmpDerivativeInput);
                    Cube::add(*pDerivativeInput,tmpDerivativeInput,error.gradLogDensity(pModel->output()));
                    return true;
                };
            InputDerivativeType *pDerivativeInput;
            InputDerivativeType tmpDerivativeInput;
            ModelT *pModel;
        };
        virtual bool computeDerivativeInput(InputDerivativeType &derivativeInput)
            {
                mpModel->setParameter(*this->mpInput);
                Array<InputT,TargetT> data(*mpInputs,*mpTargets);
                Cube::setStructure(derivativeInput,*this->mpInput);
                DerivativeBlock derivativeB(*mpModel,derivativeInput);
                data.each(derivativeB);
                if (mPrior)
                {
                    vector<double> input, gradPrior;
                    Cube::flatten(input, *this->mpInput);
                    mPrior->gradLogDensity(gradPrior, input);
                    Cube::add(derivativeInput, gradPrior);
                }
                return true;
            }

    private:
        vector<InputT> *mpInputs;
        vector<TargetT> *mpTargets;
        ModelT *mpModel;
        PriorT *mPrior;
    };
}

#endif
