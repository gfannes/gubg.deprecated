#include <cmath>
#include <vector>
#include <iostream>
using namespace std;
#define LOG(m) cout << m << endl

typedef vector<double> Array;

struct Neuron
{
    double bias;
    Array weights;
    void setBias(double b){bias = b;}
    double compute(const Array &inputs)
    {
        double res = bias;
        for (int i = 0; i < weights.size(); ++i)
            res += weights[i]*inputs[i];
        res = 1.0-2.0/(1.0+::exp(res+res));
        return res;
    }
};
struct Neurons
{
    vector<Neuron> neurons;
    Neurons(int nr):
        neurons(nr) { }
    void setBiasAndWeights(double bias, const Array &weights)
    {
#if 0
        for (int i = 0; i < neurons.size(); ++i)
        {
            neurons[i].bias = bias;
            neurons[i].weights = weights;
        }
#else
        for (auto neuron = neurons.begin(); neuron != neurons.end(); ++neuron)
        {
            neuron->bias = bias;
            neuron->weights = weights;
        }
#endif
    }
    double compute(const Array &inputs)
    {
        double d = 0;
#if 0
        for (int i = 0; i < neurons.size(); ++i)
        {
            d = neurons[i].compute(inputs);
        }
#else
        for (auto neuron = neurons.begin(); neuron != neurons.end(); ++neuron)
            d = neuron->compute(inputs);
#endif
        return d;
    }
};
int main()
{
    LOG("sizeof(Neuron): " << sizeof(Neuron));
    Neurons neurons(30000000);
    Array weights = {0.1, 0.2, 0.3, 0.4};
    neurons.setBiasAndWeights(0.5, weights);
    Array inputs = {1, 2, 3, 4};
    double d = 0;
    for (int i = 0; i < 10; ++i)
    {
        LOG("Iteration " << i);
        d += neurons.compute(inputs);
    }
    LOG("D: " << d);
    LOG("Finished");
    return 0;
}
