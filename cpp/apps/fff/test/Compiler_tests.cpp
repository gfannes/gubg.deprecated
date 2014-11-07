#include "fff/Compiler.hpp"
#include "catch/catch.hpp"
using namespace fff;

TEST_CASE("constructor", "[Value]")
{
    Compiler cc(compiler::Type::GCC);
}
