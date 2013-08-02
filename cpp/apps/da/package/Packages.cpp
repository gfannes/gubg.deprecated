#include "da/package/Packages.hpp"
#include <algorithm>
#include <set>
using namespace da::package;
using namespace std;

#define GUBG_MODULE_ "Packages"
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
	MSS_BEGIN(ReturnCode, partial);
    for (auto pkg: packages_)
    {
        switch (auto rc = pkg->resolveHeader(resolvedHeader, sisterFiles, partial)) 
		{
			case ReturnCode::OK:
				L("Header " << partial << " was resolved by " << pkg->name() << " into " << resolvedHeader);
				MSS_RETURN_OK();
				break;
			case ReturnCode::UnknownHeader:
				break;
			default:
				L(pkg->name() << " failed on " << partial);
				MSS(rc);
				break;
		}
    }
	L(partial << " is unknown by everybody");
	MSS(ReturnCode::UnknownHeader);
	MSS_END();
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
