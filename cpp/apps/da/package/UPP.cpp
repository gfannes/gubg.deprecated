#include "da/package/UPP.hpp"
#include "gubg/file/Filesystem.hpp"
#include <vector>
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

UPP::UPP(){}
UPP::UPP(const File &base):
    base_(base)
{
}

bool UPP::exists() const
{
    return gubg::file::exists(base_);
}
void UPP::expandForest(Forest &forest) const
{
    vector<string> subs = {"CtrlLib", "CtrlCore"};
    for (auto sub: subs)
    {
        File dir(base_);
        dir << "uppsrc";
        dir << sub;
        forest.add(dir, {"cpp", "h"});
    }
}
void UPP::appendIncludePaths(IncludePaths &ips) const
{
    ips.insert(File("/usr/include/freetype2"));
}
