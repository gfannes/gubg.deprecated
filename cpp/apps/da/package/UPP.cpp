#define GUBG_LOG
#include "da/package/UPP.hpp"
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
}

bool UPP::exists() const
{
    return gubg::file::exists(base_);
}
void UPP::appendIncludePaths(IncludePaths &ips) const
{
    ips.insert(File("/usr/include/freetype2"));
    ips.insert(File("/usr/include/gtk-3.0"));
    ips.insert(File("/usr/include/glib-2.0"));
}
bool UPP::resolveHeader(File &resolvedHeader, File &includePath, SourceFiles &sisterFiles, const File &partial) const
{
    LOG_S(resolveHeader, partial.name());
    File root;
    if (ReturnCode::OK != forest_.resolve(resolvedHeader, root, partial, 1))
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
