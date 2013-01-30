#include "da/Arduino.hpp"

namespace 
{
    enum Type {Unknown, Uno, Mega};
    Type type = Uno;
}

void da::arduino::setModel(const std::string &model)
{
    if (model == "uno")
        type = Uno;
    if (model == "mega")
        type = Mega;
}

bool da::arduino::isUno()
{
    return type == Uno;
}
bool da::arduino::isMega()
{
    return type == Mega;
}
