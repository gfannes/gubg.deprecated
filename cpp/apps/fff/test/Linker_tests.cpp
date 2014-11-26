#include "fff/Linker.hpp"
#include "catch/catch.hpp"
using namespace fff;
using gubg::file::File;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
TEST_CASE("constructor", "[Value]")
{
    S();
    Linker ll(linker::Vendor::GCC, linker::OutputType::Exe);
    //Linker ll(linker::Vendor::MSC, linker::OutputType::Exe);
    ll.addObject("object1");
    ll.addObject("object2");
    ll.addLibraryPath("lib_path1");
    ll.addLibraryPath("lib_path2");
    ll.addLibrary("lib1");
    ll.addLibrary("lib2");
    ll.addOption("debug");
    std::string cli;
    ll.link(cli, File("exe"));
    L(cli);
    ll.link(cli, File("exe"));
    L(cli);
}
#include "gubg/log/end.hpp"
