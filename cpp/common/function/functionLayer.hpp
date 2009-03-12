#ifndef functionLayer_h
#define functionLayer_h

#include <vector>

#include "function.hpp"

// can go
#include "vector.hpp"

namespace gubg
{
// ObjectT is the type of one object in the layer
// OutputT is the output type of ObjectT
// DerivativeT is the type of the derivative of one object
    template< typename ObjectT, typename T, int OutputDim, int InputDim >
    class FunctionLayerFromClass: public FunctionWithParameter<T, OutputDim+1, InputDim, InputDim+1 >
    {
    public:

	typedef T BaseType;
	typedef typename Dimension<T,OutputDim+1>::type OutputType;
	typedef typename Dimension<T,InputDim>::type InputType;
	typedef typename Dimension<T,InputDim+1>::type ParameterType;
	typedef typename Dimension<T,OutputDim+InputDim+1>::type InputDerivativeType;
	typedef typename Dimension<T,OutputDim+InputDim+2>::type ParameterDerivativeType;

	FunctionLayerFromClass(int nrInLayer,ObjectT &object,InputType *input = 0)
	    {
		ObjectT *pObject;
		mLayer.reserve(nrInLayer);
		this->mOutput.resize(nrInLayer);
		for (nrInLayer--;nrInLayer>=0;nrInLayer--)
		{
		    pObject = new ObjectT;
		    *pObject = object;
		    mLayer.push_back(pObject);
		}
		mOwnLayer = true;
		if (input)
		    this->mpInput = input;
	    };
	~FunctionLayerFromClass()
	    {
		if (mOwnLayer)
		{
		    typename vector<ObjectT*>::iterator it=mLayer.begin();
		    for (;it!=mLayer.end();it++)
			delete *it;
		}
	    }
	virtual bool clone(Function<T, OutputDim+1, InputDim> *&res) const
	    {
		return res = new FunctionLayerFromClass(*this);
	    }

	void desc()
	    {
		typename vector<ObjectT*>::iterator it;
		for (it=mLayer.begin();it!=mLayer.end();it++)
		{
		    cout << Vector::toString((*it)->weights()) << endl;
		}
	    }

	virtual bool computeOutput()
	    {
		bool ok = true;
		long i,nr=mLayer.size();
		for (i=0;ok && i<nr;i++)
		{
		    if (this->mpParameter)
			ok &= mLayer[i]->setParameter4Function((*this->mpParameter)[i]);
		    ok &= mLayer[i]->output4Function(this->mOutput[i],*this->mpInput);
		}
		return ok;
	    };

	virtual bool computeDerivativeInput(InputDerivativeType &derivative)
	    {
		derivative.resize(mLayer.size());
		for (int i=0; i<mLayer.size(); ++i)
		{
		    if (!mLayer[i]->derivativeInput4Function(derivative[i], *this->mpInput))
		    {
			cerr << "Could not compute the inputparameter derivative in FunctionLayer" << endl;
			return false;
		    }
		}
		return true;            
	    }

	virtual bool computeDerivativeParameter(ParameterDerivativeType &derivative)
	    {
		derivative.resize(mLayer.size());
		for (int i=0; i<mLayer.size(); ++i)
		{
		    derivative[i].resize(mLayer.size());
		    if (!mLayer[i]->derivativeParameter4Function(derivative[i][i], *this->mpInput))
		    {
			cerr << "Could not compute the parameter derivative in FunctionLayer" << endl;
			return false;
		    }
		    for (int j=0; j<mLayer.size(); ++j)
		    {
			if (i != j)
			{
			    Cube::setStructure(derivative[i][j], derivative[i][i]);
			    Cube::setSame(derivative[i][j], 0.0);
			}
		    }
		}
		return true;
	    }
	// Global output has dimension 1
	virtual bool multiplyDerivativeParameter(ParameterDerivativeType &res, const InputDerivativeType &rhs)
	    {
		res.resize(rhs.size());
		for (int i=0; i<rhs.size(); ++i)
		{
		    res[i].resize(mLayer.size());
		    for (int k=0; k<mLayer.size(); ++k)
		    {
			typename Dimension<BaseType, InputDim+OutputDim>::type derP;
			if (!mLayer[k]->derivativeParameter4Function(derP, *this->mpInput))
			{
			    cerr << "Could not compute the parameter derivative in FunctionLayer" << endl;
			    return false;
			}
			if (!Cube::multiplyRight(res[i][k], derP, rhs[i][k]))
			{
			    cerr << "Could not perform this multiplication" << endl;
			    return false;
			}
		    }
		}
		return true;
	    }
	// Global output has dimension 0
	virtual bool multiplyDerivativeParameter(ParameterType &res, const InputType &rhs)
	    {
		res.resize(mLayer.size());
		for (int k=0; k<mLayer.size(); ++k)
		{
		    typename Dimension<BaseType, InputDim+OutputDim>::type derP;
		    if (!mLayer[k]->derivativeParameter4Function(derP, *this->mpInput))
		    {
			cerr << "Could not compute the parameter derivative in FunctionLayer" << endl;
			return false;
		    }
		    if (!Cube::multiplyRight(res[k], derP, rhs[k]))
		    {
			cerr << "Could not perform this multiplication" << endl;
			return false;
		    }
		}
		return true;
	    }

    private:
	vector<ObjectT*> mLayer;
	bool mOwnLayer;
    };
};

#endif
