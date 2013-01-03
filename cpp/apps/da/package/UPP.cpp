#include "da/package/UPP.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace da::package;
using namespace gubg::file;

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
    File uppsrc(base_);
    uppsrc << "uppsrc/CtrlLib";
    forest.add(uppsrc, {"cpp", "h"});
}
void UPP::appendIncludePaths(IncludePaths &ips) const
{
    ips.push_back(File("/usr/include/freetype2"));
}
