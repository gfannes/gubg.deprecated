#ifndef mlp_h
#define mlp_h

#include <vector>

#include "vector.hpp"
#include "function.hpp"
#include "functionLayer.hpp"
#include "functionChain.hpp"
#include "neuron.hpp"

using namespace std;

namespace gubg
{
    using namespace Vector;

    template <typename T>
    struct MLP_Traits
    {
	typedef FunctionLayerFromClass<Neuron<eNeuronTanh>, double, 0, 1> NeuronLayerTanhT;
	typedef FunctionLayerFromClass<Neuron<eNeuronSigmoid>, double, 0, 1> NeuronLayerSigmoidT;
	typedef FunctionLayerFromClass<Neuron<eNeuronLinear>, double, 0, 1> NeuronLayerLinearT;

	static T& convertOutput(vector<double> &output){return output;}
	static FunctionWithParameter<double, 1, 1, 2>* createLayer(NeuronE nType, int nrInputs, int nrOutputs)
	    {
		Neuron<eNeuronTanh> neuronTanh;
		Neuron<eNeuronSigmoid> neuronSigmoid;
		Neuron<eNeuronLinear> neuronLinear;

		NeuronLayerTanhT *pTanhLayer;
		NeuronLayerSigmoidT *pSigmoidLayer;
		NeuronLayerLinearT *pLinearLayer;

		vector<double> weights;
		Vector::setUniform(weights,nrInputs+1,-1,1);
		switch (nType)
		{
		case eNeuronTanh:
		    neuronTanh.setWeights(weights);
		    pTanhLayer = new NeuronLayerTanhT(nrOutputs, neuronTanh);
		    return pTanhLayer;
		    break;
		case eNeuronSigmoid:
		    neuronSigmoid.setWeights(weights);
		    pSigmoidLayer = new NeuronLayerSigmoidT(nrOutputs, neuronSigmoid);
		    return pSigmoidLayer;
		    break;
		case eNeuronLinear:
		    neuronLinear.setWeights(weights);
		    pLinearLayer = new NeuronLayerLinearT(nrOutputs, neuronLinear);
		    return pLinearLayer;
		    break;
		default:
		    cerr << "Unknown neuron type" << endl;
		    return NULL;
		}
	    }
    };
    template <>
    struct MLP_Traits<double>
    {
	typedef FunctionFromClass<Neuron<eNeuronTanh, 2>, double, 0, 1, 2> NeuronTanhT;
	typedef FunctionFromClass<Neuron<eNeuronSigmoid, 2>, double, 0, 1, 2> NeuronSigmoidT;
	typedef FunctionFromClass<Neuron<eNeuronLinear, 2>, double, 0, 1, 2> NeuronLinearT;

	static double& convertOutput(vector<double> &output){return output.front();}
	static FunctionWithParameter<double, 0, 1, 2>* createLayer(NeuronE nType, int nrInputs, int nrOutputs)
	    {
		if (nrOutputs != 1)
		{
		    cerr << "I expect 1 output when you want to have a double as output of the MLP" << endl;
		    return NULL;
		}

		Neuron<eNeuronTanh, 2> *neuronTanh;
		Neuron<eNeuronSigmoid, 2> *neuronSigmoid;
		Neuron<eNeuronLinear, 2> *neuronLinear;

		NeuronTanhT *pTanh;
		NeuronSigmoidT *pSigmoid;
		NeuronLinearT *pLinear;

		vector<double> weights;
		Vector::setUniform(weights,nrInputs+1,-1,1);
		switch (nType)
		{
		case eNeuronTanh:
		    neuronTanh = new Neuron<eNeuronTanh, 2>;
		    neuronTanh->setWeights(weights);
		    pTanh = new NeuronTanhT(*neuronTanh, true);
		    return pTanh;
		    break;
		case eNeuronSigmoid:
		    neuronSigmoid = new Neuron<eNeuronSigmoid, 2>;
		    neuronSigmoid->setWeights(weights);
		    pSigmoid = new NeuronSigmoidT(*neuronSigmoid, true);
		    return pSigmoid;
		    break;
		case eNeuronLinear:
		    neuronLinear = new Neuron<eNeuronLinear, 2>;
		    neuronLinear->setWeights(weights);
		    pLinear = new NeuronLinearT(*neuronLinear, true);
		    return pLinear;
		    break;
		default:
		    cerr << "Unknown neuron type" << endl;
		    return NULL;
		}
	    }
    };

    template <typename OutputT>
    class MLP: public FunctionWithParameter<double, Dimension<OutputT>::dimension, 1, 3>
    {
    public:
	typedef typename Dimension<double, 3>::type ParameterType;

	static const int OutputDim = Dimension<OutputT>::dimension;
	typedef typename Dimension<double, 1 + OutputDim>::type InputDerivativeType;
	typedef typename Dimension<double, 3 + OutputDim>::type ParameterDerivativeType;

	typedef FunctionWithParameter<double, 1, 1, 2> InputLayerType;
	typedef FunctionWithParameter<double, OutputDim, 1, 2> OutputLayerType;

	MLP():mOutputLayer(NULL){}
	MLP(long nrInputs, vector<int> nrPerLayer, vector<NeuronE> typePerLayer):mOutputLayer(NULL)
	    {
		setStructure(nrInputs,nrPerLayer,typePerLayer);
	    }
	virtual bool clone(Function<double,OutputDim,1> *&res) const
	    {
		return res = new MLP(*this);
	    }
	bool setStructure(long nrInputs, vector<int> nrPerLayer, vector<NeuronE> typePerLayer)
	    {
		mNrInputs = nrInputs;
		mNrPerLayer = nrPerLayer;
            
		int i;
		for (i=0;i<mInputLayers.size();i++)
		{
		    if (mInputLayers[i])
			delete mInputLayers[i];
		    mInputLayers[i]=0;
		}
		mInputLayers.resize(0);
		if (mOutputLayer)
		    delete mOutputLayer;
		mOutputLayer=0;

		// Create the input layers
		int nrOutputs;
		for (i=0;i<typePerLayer.size()-1;i++)
		{
		    nrOutputs = nrPerLayer[i];
		    mInputLayers.push_back(MLP_Traits<vector<double> >::createLayer(typePerLayer[i], nrInputs, nrOutputs));
		    nrInputs = nrOutputs;
		}

		// Create the output layer
		// Was the number of outputs specified?
		if (i < nrPerLayer.size())
		    nrOutputs = nrPerLayer[i];
		else
		    nrOutputs = 1;
		mOutputLayer = MLP_Traits<OutputT>::createLayer(typePerLayer[i], nrInputs, nrOutputs);
            
		return mChain.setStructure(mOutputLayer,&mInputLayers); 
	    }
	bool computeOutput()
	    {
		bool ok = (this->mpInput) && (this->mpParameter);
		if (ok)
		{
		    ok = mChain.setInputParameter(*this->mpInput,*this->mpParameter);
		    if (ok)
			this->mOutput = mChain.output();
//                    this->mOutput = MLP_Traits<OutputT>::convertOutput(mChain.output());
		} else
		{
		    cout << "computeOutput for MLP is not ok (input = " << this->mpInput << " and parameter = " << this->mpParameter << ")" <<  endl;
		}
		return ok;
	    }
//     bool computeDerivativeInput(InputDerivativeType &derivative){return false;}

	bool computeDerivativeParameter(ParameterDerivativeType &derivative)
	    {
		bool ok = (this->mpInput) && (this->mpParameter);
		if (ok)
		{
		    ok = mChain.setInputParameter(*this->mpInput,*this->mpParameter);
		    if (ok)
			ok = mChain.computeDerivativeParameter(derivative);
		} else
		{
		    cout << "computeOutput for MLP is not ok (input = " << this->mpInput << " and parameter = " << this->mpParameter << ")" <<  endl;
		}
		return ok;
	    }

	ParameterType *generateParameter()
	    {
		ParameterType *par;
		vector<double> w;
		vector<vector<double> > ww;
		par=new ParameterType;
		int i, j, nrI = mNrInputs, nrO;
		for (i=0;i<mNrPerLayer.size();i++)
		{
		    nrO = mNrPerLayer[i];
		    for (j=0;j<nrO;j++)
		    {
			Vector::setUniform(w,nrI+1,-1,1);
			ww.push_back(w);
		    }
		    par->push_back(ww);
		    ww.resize(0);
		    nrI = nrO;
		}
		return par;
	    }
    private:
	int mNrInputs;
	vector<int> mNrPerLayer;
	vector<InputLayerType*> mInputLayers;
	OutputLayerType *mOutputLayer;
	FunctionChain<OutputLayerType, InputLayerType> mChain;
    };
};

#endif
