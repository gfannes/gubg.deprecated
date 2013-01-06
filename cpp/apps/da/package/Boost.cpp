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
    lps.push_back(libDir_);
}
void Boost::appendLibraries(Libraries &libs) const
{
    libs.push_back("boost_thread");
    libs.push_back("boost_system");
    libs.push_back("boost_filesystem");
    libs.push_back("boost_regex");
    libs.push_back("boost_signals");
}
