#include "parsing/Structure.hpp"
using namespace meta;
using namespace std;

//#define L_ENABLE_DEBUG
#include "debug.hpp"

Structure::Structure(Code &code):
    code_(code)
{
    DEBUG_PRINT("Structure ctor");
}
Structure::~Structure()
{
    DEBUG_PRINT("Structure dtor");
}
Structure::Structure(const Structure &rhs):
    code_(rhs.code_),
    tokens_(rhs.tokens_),
    components_(rhs.components_)
{
    DEBUG_PRINT("Structure copy ctor");
}
Structure::Structure(Structure &&rhs):
    code_(std::move(rhs.code_)),
    tokens_(std::move(rhs.tokens_)),
    components_(std::move(rhs.components_))
{
    DEBUG_PRINT("Structure move ctor");
}
Structure &Structure::operator=(Structure &&rhs)
{
    DEBUG_PRINT("Structure move operator=");
    code_ = std::move(rhs.code_);
    tokens_ = std::move(rhs.tokens_);
    components_ = std::move(rhs.components_);
    return *this;
}

list<string> Structure::allNames() const
{
    list<string> res;

    for (auto token: tokens_)
    {
        auto name = dynamic_cast<Name*>(token);
        if (name)
            res.push_back(toCode(name->range_));
    }

    return std::move(res);
}

#ifdef UnitTest
#include <iostream>
int main()
{
    Code code("#include <test.h>");
    Structure s(code);
    return 0;
}
#endif
