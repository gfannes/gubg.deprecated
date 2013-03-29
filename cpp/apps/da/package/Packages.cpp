#include "da/package/Packages.hpp"
#include <algorithm>
#include <set>
using namespace da::package;
using namespace std;

#define GUBG_MODULE "Packages"
#include "gubg/log/begin.hpp"
void Packages::prune()
{
    S();
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

da::ReturnCode Packages::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    for (auto pkg: packages_)
    {
        auto rc = pkg->resolveHeader(resolvedHeader, sisterFiles, partial); 
        if (rc != ReturnCode::UnknownHeader)
            return rc;
    }
    return ReturnCode::UnknownHeader;
}

void Packages::extractCompileSettings(CompileSettings &cs) const
{
    for (auto pkg: packages_)
        if (pkg)
            pkg->extractCompileSettings(cs);
}
void Packages::extractLinkSettings(LinkSettings &ls) const
{
    for (auto pkg: packages_)
        if (pkg)
            pkg->extractLinkSettings(ls);
}

vector<string> Packages::names() const
{
    vector<string> res;
    for (auto pkg: packages_)
        if (pkg)
            res.push_back(pkg->name());
    return res;
}
