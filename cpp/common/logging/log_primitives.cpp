#include "log_primitives.hpp"

#include <iostream>

using namespace gubg;

void COutPrimitive::write(const std::string &str)
{
    std::cout << str;
}

#ifdef UNIT_TEST
int main()
{
    COutPrimitive cp;
    cp.write("asdfasdf");

    return 0;
}
#endif
