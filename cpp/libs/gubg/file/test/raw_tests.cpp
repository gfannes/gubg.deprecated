#include "gubg/file/raw_file.hpp"
#include <iostream>

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    using namespace gubg::file::raw;

    Directory root, home, gfannes;
    root.name = "";
    home.name = "home";
    gfannes.name = "gfannes";
    Regular test;
    test.name = "test.txt";

    root.add(&home);
    home.add(&gfannes);
    gfannes.add(&test);
    L(gfannes.path());
    L(test.filename());

    gfannes.expand(Recursive, NoHiddenFiles);
    size_t nr = 0;
    for (auto it = root.begin(gubg::tree::ByData); it != root.end(gubg::tree::ByData); ++it, ++nr)
    {
        L("name: " << it->name);
    }
    L("I found " << nr << " entries");
    return 0;
}
#include "gubg/log/end.hpp"
