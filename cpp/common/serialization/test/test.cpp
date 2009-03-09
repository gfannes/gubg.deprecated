#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#include "serialize.hpp"
#include "neuron.hpp"
#include "initialization.hpp"

using namespace Serialize;

int main()
{

  assert(initialize());
  Neuron<eNeuronTanh> neuron,*pNeuron;
  int type;
  ofstream fo;
  ifstream fi;
  stringstream oss;

  fo.open("data.bin", ios::out | ios::binary);
  assert(Serialize::serializeWithType(neuron,fo));
  fo.close();

  fi.open("data.bin", ios::in | ios::binary);
  assert(Serialize::unserializeWithType(pNeuron,type,fi));
  cout << "Type found: " << type << endl;
  cout << Vector::toString(pNeuron->weights()) << endl;
  fi.close();

  assert(Serialize::serialize(neuron,oss));
  cout << oss.str() << endl;

  return 0;
}
