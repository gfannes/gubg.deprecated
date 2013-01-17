//#define GUBG_LOG
#include "da/package/UPP.hpp"
#include "da/package/PkgConfig.hpp"
#include "gubg/file/Filesystem.hpp"
#include <vector>
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

UPP::UPP(){}

namespace 
{
    template <typename Forest, typename String>
    void add_(Forest &forest, gubg::file::File f, const String &str)
    {
        f << str;
        forest.add(f, {"cpp", "h"});
    }
}
UPP::UPP(const File &base):
    base_(base),
    uppsrc_(base)
{
    uppsrc_ << "uppsrc";
    add_(forest_, uppsrc_, "CtrlLib");
    add_(forest_, uppsrc_, "CtrlCore");
    add_(forest_, uppsrc_, "Core");
    add_(forest_, uppsrc_, "Draw");
    add_(forest_, uppsrc_, "RichText");
    add_(forest_, uppsrc_, "PdfDraw");
    add_(forest_, uppsrc_, "plugin/png");
    //add_(forest_, uppsrc_, "plugin");
}

bool UPP::exists() const
{
    return gubg::file::exists(base_);
}
void UPP::appendIncludePaths(IncludePaths &ips) const
{
    insertIncludePathsForPackage(ips, "glib-2.0");
    insertIncludePathsForPackage(ips, "pango");
    insertIncludePathsForPackage(ips, "cairo");
    insertIncludePathsForPackage(ips, "gtk+-3.0");
    insertIncludePathsForPackage(ips, "libssl");
}
void UPP::appendDefines(Defines &defines) const
{
    defines.insert("flagNOGTK");
}
void UPP::appendLibraries(Libraries &libraries) const
{
    insertLibrariesForPackage(libraries, "libssl");
    insertLibrariesForPackage(libraries, "x11");
    insertLibrariesForPackage(libraries, "libpng");
    libraries.insert("rt");
    libraries.insert("Xrender");
    libraries.insert("Xft");
}
bool UPP::resolveHeader(File &resolvedHeader, File &includePath, SourceFiles &sisterFiles, const File &partial) const
{
    LOG_S(resolveHeader, partial.name());
    File root;
    if (!gubg::mss::isOK(forest_.resolve(resolvedHeader, root, partial, 1)))
        return false;

    includePath = uppsrc_;

    //We add all sources of the forest
    auto files = forest_.allFiles({"cpp"});
    for (auto f: files)
    {
        LOG_M(f.name());
        sisterFiles.insert(f);
    }
    return true;
}
