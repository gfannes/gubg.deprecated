#include "gubg/cortex/Neuron.hpp"
#include <iostream>
using namespace gubg::cortex;
using namespace std;
#define LOG(m) cout << m << endl

typedef NeuronT<double, TanhTransfer> Neuron;
typedef vector<Neuron> Neurons;

int main()
{
    MSS_BEGIN(int);

    Neurons neurons(10000000);
    Neuron::InputPolicy::Weights weights = {0.1, 0.2, 0.3, 0.4};
    for (auto neuron = neurons.begin(); neuron != neurons.end(); ++neuron)
    {
        neuron->setWeights(weights);
        neuron->setBias(0.5);
    }
    Neuron::Inputs inputs = {1, 2, 3, 4};
    Neuron::FF output;
    Neuron::FF sum = 0;
    for (int i = 0; i < 10; ++i)
    {
        LOG("Iteration: " << i);
        for (auto neuron = neurons.begin(); neuron != neurons.end(); ++neuron)
            MSS(neuron->compute(output, inputs));
        sum += output;
    }
    LOG("sum: " << sum);
    MSS_END();
    LOG("Finished");
}
