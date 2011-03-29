#include "parsing/Structure.hpp"
using namespace meta;
using namespace std;

#define L_ENABLE_DEBUG
#include "debug.hpp"

Structure::Structure(const std::string &code):
    code_(code)
{
    DEBUG_PRINT("Structure ctor");
}
Structure::~Structure()
{
    DEBUG_PRINT("Structure dtor");
}
Structure::Structure(const Structure &rhs)
{
    DEBUG_PRINT("Structure copy ctor");
}
Structure::Structure(Structure &&rhs)
{
    DEBUG_PRINT("Structure move ctor");
}
Structure &Structure::operator=(const Structure &rhs)
{
    DEBUG_PRINT("Structure operator=");
    return *this;
}
Structure &Structure::operator=(Structure &&rhs)
{
    DEBUG_PRINT("Structure move operator=");
    return *this;
}

#ifdef UnitTest
#include <iostream>
int main()
{
    string code("#include <test.h>");
    Structure s(code);
    return 0;
}
#endif
