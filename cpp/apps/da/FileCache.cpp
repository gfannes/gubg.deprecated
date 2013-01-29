#include "da/FileCache.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include <iostream>
using namespace da;
using namespace std;

FileCache::FileCache()
{
    string dir_str;
    if (!gubg::env::expand(dir_str, "$GUBG_TMP/da"))
    {
        cout << "Could not determine the cache directory" << endl;
        return;
    }
    File dir(dir_str);
    if (!gubg::file::isDirectory(dir))
    {
        cout << "Cache directory \"" << dir.name() << "\" does not exist" << endl;
        return;
    }
    dir_ = dir;
}
