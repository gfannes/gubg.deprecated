#include "da/package/Packages.hpp"
#include "gubg/l.hpp"
#include <algorithm>
#include <set>
using namespace da::package;
using namespace std;

void Packages::prune()
{
    //Invalidate unexisting or duplicate packages
    set<string> names;
    for (auto &pkg: packages_)
    {
        if (!pkg)
            continue;
        if (!pkg->exists())
        {
            L("Package " << pkg->name() << " does not exist");
            pkg.reset();
            continue;
        }
        auto name = pkg->name();
        if (names.count(name) == 0)
            names.insert(pkg->name());
        else
        {
            L("Package " << pkg->name() << " already included");
            pkg.reset();
        }
    }

    //Remove invalidated packages
    auto newEnd = std::remove(packages_.begin(), packages_.end(), Package::Ptr());
    if (newEnd != packages_.end())
        packages_.erase(newEnd, packages_.end());
}

bool Packages::resolveHeader(File &resolvedHeader, File &includePath, SourceFiles &sisterFiles, const File &partial) const
{
    for (auto pkg: packages_)
        if (pkg->resolveHeader(resolvedHeader, includePath, sisterFiles, partial))
            return true;
    return false;
}

void Packages::appendIncludePaths(IncludePaths &ips) const
{
    for (auto pkg: packages_)
        if (pkg)
            pkg->appendIncludePaths(ips);
}
void Packages::appendDefines(Defines &defines) const
{
    for (auto pkg: packages_)
        if (pkg)
            pkg->appendDefines(defines);
}
void Packages::appendLibraryPaths(LibraryPaths &lps) const
{
    for (auto pkg: packages_)
        if (pkg)
            pkg->appendLibraryPaths(lps);
}
void Packages::appendLibraries(Libraries &libs) const
{
    for (auto pkg: packages_)
        if (pkg)
            pkg->appendLibraries(libs);
}
#if 0
void Packages::expandForest(Package::Forest &forest) const
{
    for (auto pkg: packages_)
        if (pkg)
            pkg->expandForest(forest);
}
#endif

vector<string> Packages::names() const
{
    vector<string> res;
    for (auto pkg: packages_)
        if (pkg)
            res.push_back(pkg->name());
    return res;
}
