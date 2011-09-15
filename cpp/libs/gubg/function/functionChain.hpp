#ifndef functionChain_h
#define functionChain_h

#include <vector>

#include "function.hpp"
#include "vector.hpp"

namespace gubg
{
    using namespace Vector;

// Input layer type and the type for hidden layers are the same in this implementation
    template <typename OutputLT, typename InputLT>
    class FunctionChain: public FunctionWithParameter<typename InputLT::BaseType, Dimension<typename OutputLT::OutputType>::dimension, Dimension<typename InputLT::InputType>::dimension, Dimension<typename InputLT::ParameterType>::dimension+1 >
    {
    public:

        typedef typename InputLT::BaseType BaseType;
        typedef typename OutputLT::OutputType OutputType;
        typedef typename InputLT::InputType InputType;
        typedef vector<typename InputLT::ParameterType> ParameterType;
        typedef typename Dimension<BaseType, Dimension<OutputType>::dimension+Dimension<InputType>::dimension >::type InputDerivativeType;
        typedef typename Dimension<BaseType, Dimension<OutputType>::dimension+Dimension<ParameterType>::dimension >::type ParameterDerivativeType;
        static const int OutputDim = Dimension<typename OutputLT::OutputType>::dimension;
        static const int InputDim = Dimension<typename InputLT::InputType>::dimension;
//     typedef typename OutputLT::OutputType OutputType;
//     typedef typename InputLT::InputType InputType;
//     typedef vector<typename InputLT::ParameterType> ParameterType;
//     typedef typename InputLT::InputDerivativeType InputDerivativeType;
//     typedef vector<typename InputLT::ParameterDerivativeType> ParameterDerivativeType;

        FunctionChain():mOutputLayer(NULL){};
        FunctionChain(OutputLT *outputLayer, vector<InputLT*> *inputLayers):
            mOutputLayer(outputLayer)
            {
                if (inputLayers)
                    mInputLayers = *inputLayers;
            };
        virtual bool clone(Function<BaseType, OutputDim, InputDim> *&res) const
            {
                return res = new FunctionChain(*this);
            }
        bool setStructure(OutputLT *outputLayer, vector<InputLT*> *inputLayers)
            {
                mOutputLayer = outputLayer;
                if (inputLayers)
                    mInputLayers = *inputLayers;
                return true;
            }

        virtual bool computeOutput()
            {
                if (this->mpInput)
                {
                    InputType *input = this->mpInput;
                    long i;
                    for (i=0;i<mInputLayers.size();i++)
                    {
                        if (this->mpParameter)
                            mInputLayers[i]->setInputParameter(*input,(*this->mpParameter)[i]);
                        else
                            mInputLayers[i]->setInput(*input);
                        input = &mInputLayers[i]->output();
                    }
                    if (this->mpParameter)
                        mOutputLayer->setInputParameter(*input,(*this->mpParameter)[i]);
                    else
                        mOutputLayer->setInput(*input);
                    this->mOutput = mOutputLayer->output();
                    return true;
                }
                return false;
            };
        virtual bool computeDerivativeInput(InputDerivativeType &derivative){return false;};
    
        // When the output layer is of size larger than 1
        virtual bool computeDerivativeParameter(ParameterDerivativeType &derivative)
            {
                if (this->mpInput && this->mpParameter)
                {
                    ParameterDerivativeType derivativeTmp;
                    ParameterDerivativeType *derivativeLocal = NULL;
                    switch (OutputDim)
                    {
                    case 0:
                        derivativeLocal = &derivative;
                        break;
                    case 1:
                        derivativeLocal = &derivativeTmp;
                        break;
                    default:
                        cerr << "I cannot handle the situation with more than one output dimension yet" << endl;
                        return false;
                        break;
                    }
                
                    derivativeLocal->resize(mInputLayers.size() + 1);
                    typename OutputLT::ParameterDerivativeType derP;
                    typename OutputLT::InputDerivativeType cumulA, cumulB, *cumulN = NULL, *cumulO = NULL, *tmp;
                    typename InputLT::InputDerivativeType derI;

                    int layer = mInputLayers.size();
                    // Output layer
                    if (!mOutputLayer->computeDerivativeParameter(derP))
                    {
                        cerr << "Could not compute the parameter derivative for the output layer" << endl;
                        return false;
                    }
                    (*derivativeLocal)[layer] = derP;
                    // Hidden layers and input layer
                    for (--layer; layer>=0; --layer)
                    {
                        // After this if, cumulN points to the product of derivatives to the input of the last "layer-1" layers
                        if (!cumulN)
                        {
                            if (!mOutputLayer->computeDerivativeInput(cumulA))
                            {
                                cerr << "Could not compute the input derivatives for the output layer" << endl;
                                return false;
                            }
                            cumulN = &cumulA;
                            cumulO = &cumulB;
                        } else
                        {
                            if (!mInputLayers[layer+1]->computeDerivativeInput(derI))
                            {
                                cerr << "Could not compute the input derivative for input layer " << layer+1 << "" << endl;
                                return false;
                            }
                            tmp = cumulN;
                            cumulN = cumulO;
                            cumulO = tmp;
                            Cube::multiplyRight(*cumulN, *cumulO, derI);
                        }
                        if (!mInputLayers[layer]->multiplyDerivativeParameter((*derivativeLocal)[layer], *cumulN))
                        {
                            cerr << "Could not multiply the parameter derivative for input layer " << layer << "" << endl;
                            return false;
                        }
                    }
                    switch (OutputDim)
                    {
                    case 0:
                        break;
                    case 1:
                        Cube::transpose(derivative, *derivativeLocal);
                        break;
                    default:
                        cerr << "I cannot handle the situation with more than one output dimension yet" << endl;
                        return false;
                        break;
                    }
                    return true;
                }
                cout << "computeDerivativeParameter is not valid (mpInput = " << this->mpInput << ", mpParameter = " << this->mpParameter << ")" << endl;
                return false;
            }

    private:
        vector<InputLT*> mInputLayers;
        OutputLT *mOutputLayer;
    };
};

#endif
