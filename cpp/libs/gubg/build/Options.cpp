#include "gubg/build/Options.hpp"
using namespace gubg::build;

void Options::add(const Name &name, const Value &value)
{
    map_[name].push_back(value);
}
Options::ReturnCode getValues(Values &values, const Name &name) const
{
}
