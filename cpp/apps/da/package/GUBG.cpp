//#define GUBG_LOG
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

    File gubg(libsDir_);
    gubg << "gubg";
    forest_.add(gubg, {"cpp", "hpp"});

    File iup(libsDir_);
    iup << "iup";
    forest_.add(iup, {"cpp", "hpp"});

    File da(appsDir_);
    da << "da";
    forest_.add(da, {"cpp", "hpp"});
}

bool GUBG::exists() const
{
    return gubg::file::exists(libsDir_) && gubg::file::exists(appsDir_);
}
void GUBG::appendIncludePaths(IncludePaths &ips) const
{
    ips.insert(File("/home/gfannes/sdks/iup/include"));
    ips.insert(File("/home/gfannes/sdks/cd/include"));
}
void GUBG::appendDefines(Defines &defines) const
{
}
void GUBG::appendLibraryPaths(LibraryPaths &lps) const
{
    lps.insert(File("/home/gfannes/sdks/iup/lib/Linux35_64"));
    lps.insert(File("/home/gfannes/sdks/cd/lib/Linux35_64"));
}
void GUBG::appendLibraries(Libraries &libraries) const
{
    libraries.insert("iup");
    libraries.insert("iup_pplot");
    libraries.insert("iupcontrols");
    libraries.insert("cd");
}
bool GUBG::resolveHeader(File &resolvedHeader, File &includePath, SourceFiles &sisterFiles, const File &partial) const
{
    LOG_S(resolveHeader, partial.name());
    File root;
    if (ReturnCode::OK != forest_.resolve(resolvedHeader, root, partial, 1))
        return false;

    LOG_M(resolvedHeader.name());
    {
        std::string bn;
        if (!root.popBasename(bn))
            return false;
        if (bn == "gubg" || bn == "iup")
            includePath = libsDir_;
        else if (bn == "da")
            includePath = appsDir_;
        else
            return false;
    }
    LOG_M(includePath.name());

    {
        auto sisterFile = resolvedHeader;
        sisterFile.setExtension("cpp");
        if (forest_.contains(sisterFile))
        {
            LOG_M(sisterFile.name());
            sisterFiles.insert(sisterFile);
        }
    }
    return true;
}
