#include "parsing/Structure.hpp"
using namespace meta;
using namespace std;

Structure::Structure(const std::string &code):
    code_(code){}
Structure::~Structure()
{
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
