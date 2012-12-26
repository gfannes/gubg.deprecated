#include "da/package/GUBG.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace da::package;
using namespace gubg::file;

GUBG::GUBG(){}
GUBG::GUBG(const File &base):
    base_(base),
    libsDir_(base),
    appsDir_(base)
{
    libsDir_ << "cpp/libs";
    appsDir_ << "cpp/apps";
}

bool GUBG::exists() const
{
    return gubg::file::exists(libsDir_) && gubg::file::exists(appsDir_);
}
void GUBG::appendIncludePaths(IncludePaths &ips) const
{
    ips.push_back(libsDir_);
    ips.push_back(appsDir_);
}
void GUBG::expandForest(Forest &forest) const
{
    File gubg(libsDir_);
    gubg << "gubg";
    forest.add(gubg, {"cpp", "hpp"});
}
