#include "fff/Compiler.hpp"
#include "catch/catch.hpp"
using namespace fff;
using gubg::file::File;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
TEST_CASE("constructor", "[Value]")
{
    S();
    Compiler cc(compiler::Type::GCC);
    cc.addIncludePath("include_path1");
    cc.addIncludePath("include_path2");
    cc.addDefine("def1");
    cc.addDefine("def2");
    cc.addOption("debug");
    std::string cli;
    cc.compile(cli, File("source"), File("object"));
    L(cli);
}
#include "gubg/log/end.hpp"
