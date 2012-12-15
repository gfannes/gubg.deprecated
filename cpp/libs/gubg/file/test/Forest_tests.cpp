#define GUBG_LOG
#include "gubg/Testing.hpp"
#include "gubg/file/Forest.hpp"
#include "gubg/l.hpp"
using namespace gubg::file;

int main()
{
    TEST_TAG(main);
    Forest forest;
    TEST_OK(forest.add(File("/home/gfannes/gubg"), {"cpp", "hpp"}));
    L("\nAll files");
    for (auto f: forest.allFiles())
        L(f.name());
    L("\nAll roots");
    for (auto r: forest.allRoots())
        L(r.name());

    File resolved;
    TEST_OK(forest.resolve(resolved, File("cpp/libs/gubg/file/test/Forest_tests.cpp")));
    L(resolved.name());
}