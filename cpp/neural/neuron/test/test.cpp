#include <iostream>

#include "neuron.hpp"

using namespace std;

int main()
{
  vector<double> weights;
  Vector::set(weights,1.0,2.0,0.1);
  cout << Vector::toString(weights) << endl;

  vector<double> inputs;
  Vector::set(inputs,3.0,-1.0);
  cout << Vector::toString(inputs) << endl;

  double output;

  Neuron<eNeuronTanh> neuron(weights);
  neuron.output4Function(output,inputs);
  cout << output << endl;

  return 0;

}
