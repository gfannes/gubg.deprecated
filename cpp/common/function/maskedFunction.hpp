#ifndef maskedFunction_h
#define maskedFunction_h

#include "function.hpp"
#include "matrix.hpp"

template <typename T, int OutputDim, int InputDim, int ParameterDim, typename FunctionT>
class MaskedFunction: public FunctionWithParameter<T, OutputDim, InputDim, ParameterDim>
{
public:
    typedef typename Dimension<T,OutputDim+InputDim>::type InputDerivativeType;
    typedef typename Dimension<T,OutputDim+ParameterDim>::type ParameterDerivativeType;
//     typedef vector<vector<vector<T> > > ParameterType;
//     typedef vector<T> InputDerivativeType;
//     typedef vector<vector<vector<OutputT > > > ParameterDerivativeType;

    MaskedFunction(FunctionT &function,Matrix<bool> &mask):mFunction(&function),mMask(&mask){};
    virtual bool clone(Function<T,OutputDim,InputDim> *&res) const
        {
            return res = new MaskedFunction(*this);
        }
    bool computeOutput()
        {
//             cout << "Compute for masked" << endl;
            bool ok = (this->mpInput) && (this->mpParameter);
            if (ok)
            {
                Matrix<T> matrix;
                matrix = *this->mpInput;
                long nrR = matrix.nrRows() - mMask->nrRows(),nrC = matrix.nrCols() - mMask->nrCols();
                vector<T> inputs;
                vector<T> outputs;
                vector<int> rixs;
                vector<int> cixs;
                for (long i = 0; ok && i<nrR; i++)
                {
                    for (long j = 0; ok && j<nrC; j++)
                    {
                        matrix.match(inputs,*mMask,i,j);
                        ok = ((i==0 && j==0) ? 
                              mFunction->setInputParameter(inputs,*this->mpParameter) :
                              mFunction->setInput(inputs));
                        if (ok)
                        {
                            outputs.push_back(mFunction->output());
                            rixs.push_back(i);
                            cixs.push_back(j);
                        } else
                            cerr << "Some problem occurred" << endl;
                    }
                }
                T maxx;
                if (ok)
                {
                    for (long i = 0; i<outputs.size(); i++)
                    {
                        if (i==0 || outputs[i] > maxx)
                        {
                            maxx = outputs[i];
//                             cout << "Found maximum " << maxx << " out of " << outputs.size() << " at IX " << i << "" << endl;
                            mRIX = rixs[i];
                            mCIX = cixs[i];
                        }                    }
                    this->mOutput = maxx;
                }
            } else
            {
                cout << "computeOutput for MaskedFunction is not ok (input = " << this->mpInput << " and parameter = " << this->mpParameter << ")" <<  endl;
            }
            return ok;
        };
    bool computeDerivativeInput(InputDerivativeType &derivative){return false;};
    bool computeDerivativeParameter(ParameterDerivativeType &derivative)
        {
//             cout << "Derivative for masked" << endl;
            Matrix<T> matrix;
            matrix = *this->mpInput;
            vector<T> inputs;
//             cout << "Winning (" << this << ") == (" << mRIX << ", " << mCIX << ")" << endl;
            matrix.match(inputs,*mMask,mRIX,mCIX);
            mFunction->setInput(inputs);
            return mFunction->computeDerivativeParameter(derivative);
        }
private:
    FunctionT *mFunction;
    Matrix<bool> *mMask;
    int mRIX;
    int mCIX;
};

#endif
