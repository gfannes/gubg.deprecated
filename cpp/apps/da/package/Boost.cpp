#include "da/package/Boost.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace da::package;
using namespace gubg::file;

Boost::Boost(){}
Boost::Boost(const File &base):
    base_(base),
    libDir_(base)
{
    libDir_ << "stage/lib";
}

bool Boost::exists() const
{
    return gubg::file::exists(base_);
}
void Boost::appendIncludePaths(IncludePaths &ips) const
{
    ips.insert(base_);
}
void Boost::appendLibraryPaths(LibraryPaths &lps) const
{
    lps.insert(libDir_);
}
void Boost::appendLibraries(Libraries &libs) const
{
    libs.insert("boost_thread");
    libs.insert("boost_system");
    libs.insert("boost_filesystem");
    libs.insert("boost_regex");
    libs.insert("boost_signals");
}
