#include <string>
#include <iostream>
#include <sstream>

#include "comPoint.hpp"

using namespace std;

string ComPoint::toStr()
{
    ostringstream str;
    str << "Compoint " << this << " points to " << mHostname << ":" << mPort << endl;
    return str.str();
}
