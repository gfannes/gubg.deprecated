#ifndef gubg_neural_neuron_h
#define gubg_neural_neuron_h

#include <cmath>
#include <cassert>

#include "serialize.hpp"
#include "vector.hpp"

using namespace std;
using namespace gubg;

// Transfer function types
enum NeuronE
{
    eNeuronTanh,
    eNeuronSigmoid,
    eNeuronLinear
};


template <int weightDim>
struct Neuron_Traits
{
    typedef vector<double> WeightType;
    static vector<double>& getWeights(WeightType &ws){return ws;}
    static bool setWeights(WeightType &ows, const vector<double> &iws)
        {
            ows = iws;
            return true;
        }
};
template <>
struct Neuron_Traits<2>
{
    typedef vector<vector<double> > WeightType;
    static vector<double>& getWeights(WeightType &ws)
        {
            if (ws.size() == 0)
                ws.resize(1);
            return ws[0];
        }
    static bool setWeights(WeightType &ows, const vector<double> &iws)
        {
            if (ows.size() == 0)
                ows.resize(1);
            ows[0] = iws;
            return true;
        }
};

// Neuron with default tangents hyperbolicus transfer function
template< NeuronE type = eNeuronTanh, int weightDim = 1 >
class Neuron
{
public:
    typedef typename Neuron_Traits<weightDim>::WeightType WeightType;

    Neuron(){};
    Neuron(vector<double> &weights)
        {
            Neuron_Traits<weightDim>::setWeights(mWeights, weights);
        };
    Neuron(vector<vector<double> > &weights)
        {
            Neuron_Traits<weightDim>::setWeights(mWeights, weights[0]);
        };
    ~Neuron()
        {
            // if (!mWeights.empty()) // do(0)
                // cout << "Destructing neuron with data " << &mWeights[0] << endl; // do(0)
        }
    Neuron(const Neuron &source)
        {
            mWeights = source.mWeights;
            // if (!mWeights.empty()) // do(0)
                // cout << "Copying neuron with data " << &source.mWeights[0] << " to " << &mWeights[0] << endl; // do(0)
        }
    Neuron& operator=(const Neuron &source)
        {
            mWeights = source.mWeights;
            // if (!mWeights.empty()) // do(0)
                // cout << "Assigning neuron with data " << &source.mWeights[0] << " to " << &mWeights[0] << endl; // do(0)
            return *this;
        }

    WeightType &weights(){return mWeights;};
    vector<double> &coreWeights(){return Neuron_Traits<weightDim>::getWeights(mWeights);}
    int nrWeights(){return coreWeights().size();}
    static inline double transferFunction(double x)
         {
            switch (type)
            { 
            case eNeuronTanh:
                return 1.0-2.0/(1.0+exp(x+x));
            case eNeuronSigmoid:
                return 1.0/(1.0+exp(-x));
            case eNeuronLinear:
                return x;
            default:
                cerr << "Unknown neuron type %d " << type << endl;
            }
            return NAN;
        }
    static inline double derivativePostAct(double y)
         {
            switch (type)
            { 
            case eNeuronTanh:
                return (1.0+y)*(1.0-y);
            case eNeuronSigmoid:
                return y*(1.0-y);
            case eNeuronLinear:
                return 1.0;
            default:
                cerr << "Unknown neuron type %d " << type << endl;
            }
            return NAN;
        }

    // Methods for serialization
    static Neuron<type>* allocate4Serialize(){return (new Neuron<type>);}
    bool serialize4Serialize(ostream &output){return Serialize::serialize(coreWeights(),output);}
    bool unserialize4Serialize(istream &input){return Serialize::unserialize(coreWeights(),input);}
    
    // Methods for function
    bool output4Function(double &output,vector<double> &input)
        {
            int nr=nrWeights();
            vector<double> &weights = coreWeights();
            if (nr!=input.size()+1)
            {
                cerr << "The size of the input vector (" << input.size() << " + 1) does not correspond with the weights (" << nr << ")" << endl;
                output=NAN;
                return false;
            }
            output=weights[--nr];
            for (nr--;nr>=0;nr--)
                output += weights[nr]*input[nr];
            output=transferFunction(output);
            return true;
        }
    bool derivativeInput4Function(vector<double> &output,vector<double> &input)
        {
            double derPostAct;

            if (!output4Function(derPostAct,input))
            {
                cerr << "Could not compute the output in derivativeInput4Function" << endl;
                return false;
            }
            derPostAct=derivativePostAct(derPostAct);

            int nr=nrWeights()-1;
            vector<double> &weights = coreWeights();
            output.resize(nr);
            for (nr--;nr>=0;nr--)
                output[nr] = derPostAct * weights[nr];

            return true;
        }
    // output is of type WeightType
    bool derivativeParameter4Function(WeightType &output,vector<double> &input)
        {
            double derPostAct;

            if (!output4Function(derPostAct,input))
            {
                cerr << "Could not compute the output in derivativeParameter4Function" << endl;
                return false;
            }
            derPostAct=derivativePostAct(derPostAct);

            int nr=nrWeights();
            vector<double> &weights = coreWeights();
            vector<double> &coreOutput = Neuron_Traits<weightDim>::getWeights(output);
            coreOutput.resize(nr);
            coreOutput[--nr]=derPostAct;
            for (nr--;nr>=0;nr--)
                coreOutput[nr] = derPostAct * input[nr];
            
            return true;
        }
    
    bool setWeights(vector<double> &weights)
        {
            Neuron_Traits<weightDim>::setWeights(mWeights, weights);
            return true;
        }
    bool setWeights(vector<vector<double> > &weights)
        {
            Neuron_Traits<weightDim>::setWeights(mWeights, weights[0]);
            return true;
        }
    bool setParameter4Function(WeightType &weights){return setWeights(weights);};
private:
    WeightType mWeights;
};

namespace gubg
{
    namespace Serialize
    {
	SetType(Neuron<eNeuronTanh>,eTypeIDNeuronTanh);
    }
}

#endif
