#include "gubg/file/basic.hpp"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#define LOG(msg) cout << msg << endl

int main()
{
    vector<string> testPaths = {
        "/", "//", "///", "/../../../../",
        "./", "././././////",
        "/home/gfannes", "~", "~/", "~/gubg", "~/gubg/", "~/gubg/..", "~/../gfannes"
    };
    for (auto path = testPaths.begin(); path != testPaths.end(); ++path)
    {
        string pathE;
        gubg::file::basic::expandPath(pathE, *path);
        LOG("path: " << *path << ", pathE: " << pathE);
    }

    gubg::file::basic::Directories dirs;
    gubg::file::basic::Files files;
    gubg::file::basic::getDirectoryContent(dirs, files, "./");
    for (auto d = dirs.begin(); d != dirs.end(); ++d)
        LOG("Dir: " << *d);
    for (auto f = files.begin(); f != files.end(); ++f)
        LOG("File: " << *f);
    return 0;
}
