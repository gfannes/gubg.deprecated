#include "gubg/file/raw_file.hpp"
#include <iostream>
#define LOG(msg) std::cout << msg << std::endl

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
    LOG(gfannes.path());
    LOG(test.filename());
    return 0;
}
