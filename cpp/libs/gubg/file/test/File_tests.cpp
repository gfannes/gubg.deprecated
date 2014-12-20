#include "catch/catch.hpp"
#include "gubg/file/File.hpp"
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
TEST_CASE("Ctor", "[file]")
{
    SECTION("Default")
    {
        File file;
        REQUIRE(file.name().empty());
        REQUIRE(File::Unknown == file.type());
    }
    SECTION("From const char *")
    {
        File file("abc");
        REQUIRE(file.name() == "abc");
        REQUIRE(File::Unknown == file.type());
    }
    SECTION("From std::string")
    {
        File file(string("abc"));
        REQUIRE(file.name() == "abc");
        REQUIRE(File::Unknown == file.type());
    }
    SECTION("From const char * with type")
    {
        File file("abc", File::Regular);
        REQUIRE(file.name() == "abc");
        REQUIRE(File::Regular == file.type());
    }
    SECTION("From std::string with type")
    {
        File file(string("abc"), File::Regular);
        REQUIRE(file.name() == "abc");
        REQUIRE(File::Regular == file.type());
    }
}
TEST_CASE("Setters", "[file]")
{
    File file;
    file.setName("abc");
    REQUIRE(file.name() == "abc");
    REQUIRE(File::Unknown == file.type());
    file.setType(File::Regular);
    REQUIRE(File::Regular == file.type());
    file.setName("ABC").setType(File::Directory);
    REQUIRE(file.name() == "ABC");
    file.setType(File::Directory);
}
TEST_CASE("Append", "[file]")
{
    File file;
    file << "a";
    REQUIRE(file.name() == "a");
    file << "b";
    REQUIRE(file.name() == "a/b");
    file << "/c";
    REQUIRE(file.name() == "a/b/c");
    file << "";
    REQUIRE(file.name() =="a/b/c");
}
TEST_CASE("Extension", "[file]")
{
    File f("a/b/c");
    REQUIRE(f.name() == "a/b/c");
    f.setExtension("obj");
    REQUIRE(f.name() == "a/b/c.obj");
    f.setExtension("obj");
    REQUIRE(f.name() == "a/b/c.obj");
    f.setExtension("obj2");
    REQUIRE(f.name() == "a/b/c.obj2");
}
#include "gubg/log/end.hpp"
