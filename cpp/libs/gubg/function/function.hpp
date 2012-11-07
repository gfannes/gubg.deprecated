#ifndef function_h
#define function_h

#include <iostream>

#include "vector.hpp"
#include "cube.hpp"

using namespace std;

namespace gubg
{
    using namespace Vector;
    using namespace Cube;
    template <typename T,int OutputD, int InputDim>
        class Function
        {
            public:
                Function():
                    mpInput(0),
                    mInputChanged(true){};

                typedef T BaseType;
                typedef typename Dimension<T,OutputD>::type OutputType;
                static const int OutputDim = OutputD;
                typedef typename Dimension<T,InputDim>::type InputType;
                typedef typename Dimension<T,OutputD+InputDim>::type InputDerivativeType;

                // This function should be provided by the implementation, computing mOutput based on the input value mpInput and possibly other things
                virtual bool computeOutput() = 0;
                // This cloning method. Put somthing like this: return res = new <class name>(*this);
                // Make sure you have the const after the prototype
                virtual bool clone(Function *&res) const = 0;
                // derivativeInput should get filled with the derivative wrt the inputs
                virtual bool computeDerivativeInput(InputDerivativeType &derivativeInput)
                {
                    return computeNumericDerivativeInput(derivativeInput);
                }
                // The first dimensions follow the output, the deeper nested dimensions follow the input
                virtual bool computeNumericDerivativeInput(InputDerivativeType &derivativeInput)
                {
                    cout << "You are using numerical derivatives in Function<>" << endl;
                    BaseType eps=0.0000001, tmp;
                    OutputType origOutput;
                    if (mpInput)
                    {
                        computeOutput();
                        origOutput = mOutput;
                        Iterator<BaseType,InputDim> iterI(*mpInput);
                        setStructure(derivativeInput,mOutput,*mpInput);
                        for ( ; iterI.valid() ; ++iterI)
                        {
                            // cout << "Computing derivative wrt input " << toString(pathI) << "" << endl; // do(0)
                            tmp = *iterI;
                            *iterI += eps;
                            mInputChanged = true;
                            computeOutput();
                            Iterator<BaseType,OutputD> iterO(mOutput);
                            Iterator<BaseType,OutputD> iterOO(origOutput);
                            Iterator<InputType,OutputD> iterDO(derivativeInput);
                            for ( ; iterO.valid() ; ++iterO,++iterOO,++iterDO)
                            {
                                iterI.change(*iterDO);
                                (*iterI) = ((*iterO) - (*iterOO))/eps;
                            }
                            iterI.change(*mpInput);
                            *iterI = tmp;
                        }
                        computeOutput();
                    } else
                    {
                        cerr << "No input given to compute the numerical derivative to the inputs" << endl;
                        return false;
                    }
                    return true;
                }
                // The first dimensions follow the input, the deeper nested dimensions follow the output
                virtual bool computeNumericDerivativeInputDeprecated(InputDerivativeType &derivativeInput)
                {
                    cout << "You are using numerical derivatives in Function<>" << endl;
                    BaseType eps=0.0000001,tmp;
                    OutputType origOutput;
                    if (mpInput)
                    {
                        computeOutput();
                        origOutput = mOutput;
                        Iterator<BaseType,InputDim> iterI(*mpInput);
                        setStructure(derivativeInput,*mpInput,mOutput);
                        Iterator<OutputType,InputDim> iterDI(derivativeInput);
                        for ( ; iterI.valid() ; ++iterI,++iterDI)
                        {
                            // cout << "Computing derivative wrt input " << toString(pathI) << "" << endl; // do(0)
                            tmp=*iterI;
                            (*iterI)+=eps;
                            mInputChanged = true;
                            computeOutput();
                            Iterator<BaseType,OutputD> iterO(mOutput);
                            Iterator<BaseType,OutputD> iterOO(origOutput);
                            Iterator<BaseType,OutputD> iterDO(*iterDI);
                            for ( ; iterO.valid() ; ++iterO,++iterOO,++iterDO)
                            {
                                (*iterDO) = ((*iterO) - (*iterOO))/eps;
                            }
                            (*iterI) = tmp;
                        }
                        computeOutput();
                    } else
                    {
                        cerr << "No input given to compute the numerical derivative to the inputs" << endl;
                        return false;
                    }
                    return true;
                }

                // Output functions
                virtual OutputType& output(){return mOutput;};
                virtual OutputType& output(InputType &input)
                {
                    setInput(input);
                    return mOutput;
                };

                // Input functions
                virtual InputType* input(){return mpInput;};
                virtual bool setInput(InputType &input)
                {
                    mpInput = &input;
                    mInputChanged = true;
                    return computeOutput();
                };
                void invalidateInput()
                {
                    mpInput = NULL;
                    mInputChanged = true;
                };

                // Derivative functions
                bool derivativeInput(InputDerivativeType *&pDerivative)
                {
                    pDerivative = &mDerivativeInput;
                    if (!computeDerivativeInput(mDerivativeInput))
                    {
                        cerr << "Could not compute the derivative wrt the inputs" << endl;
                        return false;
                    }
                    return true;
                }

                long outputDimension(){return Dimension<OutputType>::dimension;};
                long nrInputs()
                {
                    if (mpInput)
                        return nrElements(*mpInput);
                    return -1;
                };

            protected:
                OutputType mOutput;

                InputType *mpInput;
                bool mInputChanged;

                InputDerivativeType mDerivativeInput;
        };

    template <typename T, int OutputD, int InputDim, int ParameterDim>
        class FunctionWithParameter: public Function<T, OutputD, InputDim>
    {
        public:
            FunctionWithParameter():
                mpParameter(NULL),
                mParameterChanged(true){};

            typedef T BaseType;
            typedef typename Dimension<T,OutputD>::type OutputType;
            typedef typename Dimension<T,InputDim>::type InputType;
            typedef typename Dimension<T,ParameterDim>::type ParameterType;
            typedef typename Dimension<T,OutputD+InputDim>::type InputDerivativeType;
            typedef typename Dimension<T,OutputD+ParameterDim>::type ParameterDerivativeType;

            // derivativeParameter should get filled with the derivative wrt the parameters
            virtual bool computeDerivativeParameter(ParameterDerivativeType &derivativeParameter)
            {
                return computeNumericDerivativeParameter(derivativeParameter);
            }
            // The first dimensions follow the output, the deeper nested dimensions follow the parameters
            virtual bool computeNumericDerivativeParameter(ParameterDerivativeType &derivativeParameter)
            {
                cout << "You are using numerical derivatives in FunctionWithParameter<>" << endl;
                BaseType eps=0.0000001, tmp;
                OutputType origOutput;
                if (mpParameter && this->mpInput)
                {
                    this->computeOutput();
                    origOutput = this->mOutput;
                    Iterator<BaseType,ParameterDim> iterP(*mpParameter);
                    setStructure(derivativeParameter,this->mOutput,*mpParameter);
                    for ( ; iterP.valid() ; ++iterP)
                    {
                        tmp = *iterP;
                        *iterP += eps;
                        mParameterChanged = true;
                        this->computeOutput();
                        Iterator<BaseType,OutputD> iterO(this->mOutput);
                        Iterator<BaseType,OutputD> iterOO(origOutput);
                        Iterator<ParameterType,OutputD> iterDO(derivativeParameter);
                        for ( ; iterO.valid() ; ++iterO,++iterOO,++iterDO)
                        {
                            iterP.change(*iterDO);
                            (*iterP) = ((*iterO) - (*iterOO))/eps;
                        }
                        iterP.change(*mpParameter);
                        *iterP = tmp;
                    }
                    this->computeOutput();
                } else
                {
                    cerr << "No input or parameter given to compute the numerical derivative to the parameters" << endl;
                    return false;
                }
                return true;
            }
            // The first dimensions follow the input, the deeper nested dimensions follow the output
            virtual bool computeNumericDerivativeParameterDeprecated(ParameterDerivativeType &derivativeParameter)
            {
                cout << "You are using numerical derivatives in FunctionWithParameter<>" << endl;
                BaseType eps=0.0000001,tmp;
                OutputType origOutput;
                if (mpParameter && this->mpInput)
                {
                    this->computeOutput();
                    origOutput = this->mOutput;
                    Iterator<BaseType,ParameterDim> iterP(*mpParameter);
                    setStructure(derivativeParameter,*mpParameter,this->mOutput);
                    Iterator<OutputType,ParameterDim> iterDP(derivativeParameter);
                    for ( ; iterP.valid() ; ++iterP,++iterDP)
                    {
                        tmp=*iterP;
                        (*iterP)+=eps;
                        mParameterChanged = true;
                        this->computeOutput();
                        Iterator<BaseType,OutputD> iterO(this->mOutput);
                        Iterator<BaseType,OutputD> iterOO(origOutput);
                        Iterator<BaseType,OutputD> iterDO(*iterDP);
                        for ( ; iterO.valid() ; ++iterO,++iterOO,++iterDO)
                        {
                            (*iterDO) = ((*iterO) - (*iterOO))/eps;
                        }
                        (*iterP) = tmp;
                    }
                    this->computeOutput();
                } else
                {
                    cerr << "No input or parameter given to compute the numerical derivative to the parameters" << endl;
                    return false;
                }
                return true;
            }

            // this overridable function is used in a FunctionChain to compute the derivatives wrt to the parameters, which might be done more efficiently, e.g., for FunctionLayer Functions in neural networks
            // Please note that you might need to make this a bit more general, e.g., the type of rhs is how it is currently necessary for FunctionLayers used in a FunctionChain
            // This might cause problems when previously unseen combinations of ParameterDerivativeType and InputDerivativeType are used, needing to add another dummy Cube::multiplyRight()
            virtual bool multiplyDerivativeParameter(vector<vector<vector<BaseType> > > &res, const vector<vector<BaseType> > &rhs)
            {
                cout << "You probably want to override this function (multiplyDerivativeParameter(3,2))" << endl;
                ParameterDerivativeType derP;
                if (!computeDerivativeParameter(derP))
                {
                    cerr << "Could not compute the parameter derivative for this function with parameters" << endl;
                    return false;
                }
                return Cube::multiplyRight(res, rhs, derP);
            }

            virtual bool multiplyDerivativeParameter(vector<vector<BaseType> > &res, const vector<BaseType> &rhs)
            {
                cout << "You probably want to override this function (multiplyDerivativeParameter(2,1))" << endl;
                ParameterDerivativeType derP;
                if (!computeDerivativeParameter(derP))
                {
                    cerr << "Could not compute the parameter derivative for this function with parameters" << endl;
                    return false;
                }
                return Cube::multiplyRight(res, rhs, derP);
            }

            // Output functions
            virtual OutputType& outputParameter(ParameterType &parameter)
            {
                setParameter(parameter);
                return this->mOutput;
            };

            // Input functions
            virtual ParameterType* parameter(){return mpParameter;};
            virtual bool setParameter(ParameterType &parameter)
            {
                mpParameter = &parameter;
                mParameterChanged = true;
                return this->computeOutput();
            }
            virtual bool setInputParameter(InputType &input,ParameterType &parameter)
            {
                this->mpInput = &input;
                this->mInputChanged = true;
                mpParameter = &parameter;
                mParameterChanged = true;
                return this->computeOutput();
            }

            // Derivative functions
            bool derivativeParameter(ParameterDerivativeType *&pDerivative)
            {
                pDerivative = &mDerivativeParameter;
                if (!computeDerivativeParameter(mDerivativeParameter))
                {
                    cerr << "Could not compute the derivative wrt the parameters" << endl;
                    return false;
                }
                return true;
            }
        protected:
            ParameterType *mpParameter;
            bool mParameterChanged;

            ParameterDerivativeType mDerivativeParameter;
    };

    // By default, we allocate a Function object from a class as a multi-output function
    // ObjectT is the class of the object you want to use as internal object
    // DerivativeT is the type of the derivative of the output(s) wrt to any port
    template< typename ObjectT, typename T, int OutputD, int InputDim, int ParamDim = InputDim>
        class FunctionFromClass: public FunctionWithParameter<T, OutputD, InputDim, ParamDim>
    {
        public:

            typedef T BaseType;
            typedef typename Dimension<T,OutputD>::type OutputType;
            typedef typename Dimension<T,InputDim>::type InputType;
            typedef typename Dimension<T,ParamDim>::type ParameterType;
            typedef typename Dimension<T,OutputD+InputDim>::type InputDerivativeType;
            typedef typename Dimension<T,OutputD+ParamDim>::type ParameterDerivativeType;

            FunctionFromClass(ObjectT &object, bool ownObject = false):
                mpObject(&object),
                mOwnObject(ownObject)
        {
            this->mpInput = NULL;
        };
            ~FunctionFromClass()
            {
                if (mOwnObject)
                    delete mpObject;
            }
            virtual bool clone(Function<T, OutputD, InputDim> *&res) const
            {
                return res = new FunctionFromClass(*this);
            }

            void desc(){cout << "This is multi output " << sizeof(ObjectT) << endl;};

            // These functions are not yet optimized to take into account mChangeStatus
            virtual bool computeOutput()
            {
                if (this->mpParameter && this->mpInput)
                    return mpObject->setParameter4Function(*this->mpParameter) && mpObject->output4Function(this->mOutput,*this->mpInput);
                return false;
            }

            virtual bool computeDerivativeInput(InputDerivativeType &derivativeInput)
            {
                if (this->mpInput)
                    return mpObject->derivativeInput4Function(derivativeInput,*this->mpInput);
                return false;
            };

            virtual bool computeDerivativeParameter(ParameterDerivativeType &derivativeParameter)
            {
                if (this->mpInput)
                    return mpObject->derivativeParameter4Function(derivativeParameter,*this->mpInput);
                return false;
            };
        private:
            ObjectT *mpObject;
            bool mOwnObject;
    };
};

#endif
