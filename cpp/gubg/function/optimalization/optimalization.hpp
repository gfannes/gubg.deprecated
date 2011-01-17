#ifndef optimalization_h
#define optimalization_h

#include <cmath>

#include "function.hpp"
#include "cube.hpp"
#include "block.hpp"

namespace gubg
{
    template <typename FunctionT>
    class Optimizer
    {
    public:
        typedef class: public Block<Optimizer>
        {
        public:
            bool preYield(){return true;}
            bool yield(bool &changed, FunctionT &function){return true;}
            bool postYield(FunctionT &function){return true;}
        } BlockType;

        Optimizer(long nrSteps = 10):
            mParamEps(0.0000001),
            mRoundOffError(0.0000001),
            mNrSteps(nrSteps){};

        // This method will optimize function wrt its inputs
        // A block can be passed that can modify the function (not the inputs, but other parameters)
        // It should indicate via the second argument if the function changed, and via its return value if we have to continue the optimization
        bool optimize(typename FunctionT::InputType &input, FunctionT &function, typename FunctionT::BaseType *res)
            {
                return optimize(input, function, res, (BlockType*)NULL);
            }
        template <typename B>
        bool optimize(typename FunctionT::InputType &input, FunctionT &function, typename FunctionT::BaseType *res, B *block)
            {
                cout.precision(20);
                if (function.outputDimension() != 0)
                {
                    cerr << "I can only optimize a function with zero output dimensions (and thus one output)." << endl;
                    return false;
                }
                long nrInputs = function.nrInputs();
                long lMaxSteps = nrInputs*mNrSteps;
                double dSigma0 = 1.0e-4;
                double dFnow, dFold;

                dFnow = dFold = function.output(input);

                typename FunctionT::InputDerivativeType gradnew, gradold, gradat, correction;
                if (!function.computeDerivativeInput(gradnew))
                {
                    cerr << "Could not compute the derivative wrt the inputs" << endl;
                    return false;
                }
                gradold = gradnew;
                correction = gradnew;
                bool bSuccess = true;
                double dAlpha;
                long lNrSuccess = 0;
                // Initial scale parameter.
                double dBeta = 1.0;
                //Lower bound on scale.
                double dBetaMin = 1.0e-15;
                //Upper bound on scale.
                double dBetaMax=1.0e100;
                // pInputStore is a pointer that should only be used on a short term basis
                // pAvailableInput always holds a data structure similar (or equal) to the original input argument and can be used for testing steps. In general, it will be different as the function.mpInput object.
                typename FunctionT::InputType *pInputStore, *pAvailableInput, tmpInput, inpat;
                pAvailableInput = &tmpInput;

                double dMu,dKappa,dSigma,outputBefore,dTheta,dDelta,prevAlpha,newAlpha,dFnew,dBigDelta,dGamma;
                bool functionChanged, continueOptimization;

                if (block)
                {
                    block->index = -1;
                    block->context = this;
                    block->preYield();
                }

                //Main optimization loop.
                long lStep = -1;
                while (++lStep <= lMaxSteps)
                {
                    // cout << endl << "SCG step " << lStep << " of (" << lMaxSteps << "=" << mNrSteps << "*" << nrInputs << ")" << endl; // do(2)

                    if (block)
                    {
                        ++(block->index);
                        functionChanged = false;
                        continueOptimization = block->yield(functionChanged, function);
                        if (functionChanged)
                        {
                            dFnow = dFold = dFnew = function.output(input);
                            function.computeDerivativeInput(gradnew);
                            gradold = gradnew;
                            correction = gradnew;
                            bSuccess = true;
                        }
                        if (!continueOptimization)
                        {
                            if (res)
                                *res = dFnow;
                            if (function.input() != &input)
                            {
                                input = *function.input();
                                function.setInput(input);
                            }
                            block->postYield(function);
                            // cout << "SCG exit: Block terminated" << endl; // do(2)
                            return false;
                        }
                    }

                    // cout << "\tbSuccess = " << bSuccess << endl; // do(1)
                    if (bSuccess)
                    {
                        Cube::inproduct(dMu,correction,gradnew);
                        dMu = -dMu;
                        if (dMu >= 0.0)
                        {
                            correction = gradnew;
                            Cube::inproduct(dMu,correction,gradnew);
                            dMu = -dMu;
                        }
                        Cube::inproduct(dKappa,correction,correction);
                        if (dKappa < mParamEps)
                        {
                            if (block)
                                continue;
                            else
                            {
                                if (res)
                                    *res = dFnow;
                                if (function.input() != &input)
                                {
                                    input = *function.input();
                                    function.setInput(input);
                                }
                                // cout << "SCG exit: kappa < PARAM_EPSILON" << endl; // do(2)
                                return false;
                            }
                        }
                        dSigma = dSigma0/sqrt(dKappa);
                        outputBefore=function.output();

                        pInputStore = function.input();
                        *pAvailableInput = *pInputStore;
                        Cube::update(*pAvailableInput,correction,dSigma);
                        function.setInput(*pAvailableInput);
                        // cout << "\t" << outputBefore << "\t=>1\t" << function.output() << " at " << Vector::toString(*pAvailableInput) << endl; // do(1)
                        function.computeDerivativeInput(gradat);
                        function.setInput(*pInputStore);

                        Cube::update(gradat,gradnew,-1.0);
                        // cout << Vector::toString(gradat) << endl; // do(0)
                        Cube::inproduct(dTheta,correction,gradat);
                        dTheta = -dTheta/dSigma;
                        // cout << "dTheta = " << dTheta << endl; // do(0)
                    }

                    // Increase effective curvature and evaluate step size alpha.
                    dDelta = dTheta + dBeta*dKappa;
                    // cout << "\tdelta = " << dDelta << endl; // do(0)
                    if (dDelta <= 0.0)
                    {
                        dDelta = dBeta*dKappa;
                        dBeta = dBeta - dTheta/dKappa;
                    }
                    prevAlpha=dAlpha;
                    dAlpha = - dMu/dDelta;
                    // cout << "\tprevAlhpa = " << prevAlpha << " => " << newAlpha << " = " << dAlpha << " (delta = " << dDelta << ")" << endl; // do(1)

                    // Calculate the comparison ratio.
                    outputBefore=function.output();
                    pInputStore = function.input();
                    *pAvailableInput = *pInputStore;
                    Cube::update(*pAvailableInput,correction,dAlpha);
                    function.setInput(*pAvailableInput);
                    // cout << "\t" << outputBefore << "\t=>2\t" << function.output() << endl; // do(2)
                    dFnew = function.output();
                    dBigDelta = 2.0*(dFold-dFnew)/(dAlpha*dMu);
                    // cout << "\tbigdelta = "<< dBigDelta << endl; // do(0)
                    if (dBigDelta  > 0.0)
                    {
                        pAvailableInput = pInputStore;
                        bSuccess = true;
                        lNrSuccess++;
                        dFnow = dFnew;
                    } else
                    {
                        function.setInput(*pInputStore);
                        bSuccess = false;
                        dFnow = dFold;
                    }
                    // cout << "\tsuccess is now " << bSuccess << endl; // do(1)
                
                    if (bSuccess)
                    {
                        // Test for termination
                        if ((dAlpha*Cube::l2Norm(correction) <  mRoundOffError) && (abs(dFnew-dFold) <  mRoundOffError))
                        {
                            if (block)
                                continue;
                            else
                            {
                                if (res)
                                    *res = dFnew;
                                if (function.input() != &input)
                                {
                                    input = *function.input();
                                    function.setInput(input);
                                }
                                // cout << "SCG exit: alpha*correction or stepdifference < ROUNDOFF_ERROR" << endl; // do(2)
                                return false;
                            }
                        } else
                        {
                            // Update variables for new position
                            dFold = dFnew;
                            gradold = gradnew;
                            function.computeDerivativeInput(gradnew);
                            // If the gradient is zero then we are done.
                            if (Cube::l2Norm(gradnew) <= mRoundOffError)
                            {
                                if (block)
                                    continue;
                                else
                                {
                                    if (res)
                                        *res = dFnew;
                                    if (function.input() != &input)
                                    {
                                        input = *function.input();
                                        function.setInput(input);
                                    }
                                    // cout << "SCG exit: gradient norm < ROUNDOFF_ERROR" << endl; // do(2)
                                    return true;
                                }
                            }
                        }
                    }

                    // Adjust beta according to comparison ratio.
                    if (dBigDelta < 0.25)
                        dBeta = (4.0*dBeta < dBetaMax ? 4.0*dBeta : dBetaMax);
      
                    if (dDelta > 0.75)
                        dBeta = (0.5*dBeta > dBetaMin ? 0.5*dBeta : dBetaMin) ;

                    // Update search direction using Polak-Ribiere formula, or re-start 
                    // in direction of negative gradient after nparams steps.
                    if (lNrSuccess == nrInputs-1)
                    {
                        correction = gradnew;
                        lNrSuccess = 0;
                    } else
                    {
                        if (bSuccess)
                        {
                            inpat = gradold;
                            Cube::update(inpat,gradnew,-1.0);
                            Cube::inproduct(dGamma,inpat,gradnew);
                            dGamma = dGamma/dMu;
                            Cube::multiply2All(correction,dGamma);
                            Cube::update(correction,gradnew,1.0);
                        }
                    }
                };
            
                if (res)
                    *res = dFnew;
                if (function.input() != &input)
                {
                    input = *function.input();
                    function.setInput(input);
                }
                // cout << "SCG exit: Termination because the number of steps are reached." << endl; // do(2)
                if (block)
                    block->postYield(function);
                return true;        // Not sure if we have to output true or false. We optimized something, but not to the bone
            }
    
    private:
        double mParamEps;
        double mRoundOffError;
        long mNrSteps;
    };
}

#endif
