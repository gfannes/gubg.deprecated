#include "gubg/coding/d9/parser.hpp"
#include "gubg/Testing.hpp"
#include "gubg/mss.hpp"
using namespace gubg::coding::d9;
using namespace gubg::testing;

int main()
{
    MSS_BEGIN(int);
    Parser parser;
    parser.add(0xd9);
    MSS_END();
}