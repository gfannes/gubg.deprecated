#include "message.hpp"
#include "neuron.hpp"

bool initialize()
{
  return
    Serialize::registerType<string>() &&
    Serialize::registerType<Message>() &&
    Serialize::registerType<Neuron<eNeuronTanh> >() &&
    true;
}

bool finalize()
{
  return true;
}
