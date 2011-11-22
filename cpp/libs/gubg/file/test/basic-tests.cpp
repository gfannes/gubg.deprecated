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
    return 0;
}
