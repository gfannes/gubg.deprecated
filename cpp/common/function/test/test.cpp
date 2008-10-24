#include <iostream>
#include <cmath>
#include <cassert>

#include "function.hpp"
#include "functionLayer.hpp"
#include "functionChain.hpp"

#include "vector.hpp"
#include "neuron.hpp"

enum TestCaseT
{
    eAll = 0,
    eFunction,
    eFunctionLayer,
    eFunctionChain,
};

int main()
{
//   TestCaseT testCase = eAll;
    TestCaseT testCase = eFunctionChain;
//     TestCaseT testCase = eFunction;
    Random::setSeed();

    if (!testCase || testCase==eFunction)
    {
        cout << endl << "Test case eFunction" << endl;
        vector<double> weights;
        weights.push_back(1);
        weights.push_back(2);
        weights.push_back(0.1);

        Neuron<eNeuronLinear> neuron(weights);

        vector<double> inputs;
        inputs.push_back(3);
        inputs.push_back(1);

        double output;
        vector<double> *pDerivative;

        neuron.output4Function(output,inputs);
        cout << output << endl;

        FunctionFromClass<Neuron<eNeuronLinear>,double,0,1> c1(neuron);
        c1.desc();
        c1.setInput(inputs);
        cout << "output = " << c1.output() << endl;

        weights[0]=-1;
        weights[1]=-2;
        c1.setParameter(weights);
        cout << "output = " << c1.output() << endl;

        cout << "before der" << endl;
        assert(c1.derivativeInput(pDerivative));
        cout << "after der" << endl;
        cout << "derivative: " << Vector::toString(*pDerivative) << endl;

        c1.output()=12;
        cout << c1.output() << endl;
    }
    if (!testCase || testCase==eFunctionLayer)
    {
        cout << endl << "Test case eFunctionLayer" << endl;

        vector<double> weights;
        Vector::set(weights,1.0,2.0,0.1);
        Neuron<eNeuronTanh> neuron(weights);
        vector<double> inputs;
        Vector::set(inputs,3.0,1.0);
        vector<double> inputsAlt;
        Vector::set(inputsAlt,2.0,2.0);
        FunctionLayerFromClass<Neuron<eNeuronTanh>,double,0,1> layer(3,neuron,&inputs);
        layer.desc();
        layer.setInput(inputsAlt);
        cout << Vector::toString(layer.output()) << endl;
    }
    if (!testCase || testCase==eFunctionChain)
    {
        cout << endl << "Test case eFunctionChain" << endl;

        typedef FunctionLayerFromClass<Neuron<eNeuronTanh>,double,0,1> NeuronLayerT;

        int nrI = 2, nrF = 3, nrH = 6, nrO = 1;

        vector<double> weightsI;
        Vector::setUniform(weightsI,nrI+1,-1,1);
        Neuron<eNeuronTanh> neuronI(weightsI);

        vector<double> weightsH;
        Vector::setUniform(weightsH,nrF+1,-1,1);
        Neuron<eNeuronTanh> neuronH(weightsH);

        vector<double> weightsO;
        Vector::setUniform(weightsO,nrH+1,-1,1);
        Neuron<eNeuronTanh> neuronO(weightsO);

        vector<double> inputs;
        Vector::set(inputs,3.0,1.0);
        
        NeuronLayerT inputLayer(nrF,neuronI);
        NeuronLayerT hiddenLayer(nrH,neuronH);
        NeuronLayerT outputLayer(nrO,neuronO);

        vector<NeuronLayerT*> inputLayers;
        Vector::set(inputLayers,&inputLayer,&hiddenLayer);

        FunctionChain<NeuronLayerT, NeuronLayerT> functionChain(&outputLayer,&inputLayers);

        FunctionChain<NeuronLayerT, NeuronLayerT>::ParameterType parameters;
        // 3 layers (first, hidden and output)
        parameters.resize(3);
        // first layer
        int layer = 0;
        parameters[layer].resize(nrF);
        for (int j=0; j<nrF; ++j)
            Vector::setUniform(parameters[layer][j], nrI+1, -1, 1);
        // hidden layer
        ++layer;
        parameters[layer].resize(nrH);
        for (int j=0; j<nrH; ++j)
            Vector::setUniform(parameters[layer][j], nrF+1, -1, 1);
        // output layer
        ++layer;
        parameters[layer].resize(nrO);
        for (int j=0; j<nrO; ++j)
            Vector::setUniform(parameters[layer][j], nrH+1, -1, 1);

        functionChain.setInputParameter(inputs, parameters);

        cout << Vector::toString(functionChain.output()) << endl;

        FunctionChain<NeuronLayerT, NeuronLayerT>::ParameterDerivativeType derivativeParam;
        functionChain.computeDerivativeParameter(derivativeParam);

        cout << "derivativeParam = \n" << derivativeParam << "" << endl;
    }
    return 0;
}
