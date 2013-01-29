//#define GUBG_LOG
#include "da/package/GUBG.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace da::package;
using namespace gubg::file;

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

    compileSettings_.includePaths.insert(File("/home/gfannes/sdks/iup/include"));
    compileSettings_.includePaths.insert(File("/home/gfannes/sdks/cd/include"));
    linkSettings_.libraryPaths.insert(File("/home/gfannes/sdks/iup/lib/Linux35_64"));
    linkSettings_.libraryPaths.insert(File("/home/gfannes/sdks/cd/lib/Linux35_64"));
    linkSettings_.libraries.insert("iup");
    linkSettings_.libraries.insert("iup_pplot");
    linkSettings_.libraries.insert("iupcontrols");
    linkSettings_.libraries.insert("cd");
}

bool GUBG::exists() const
{
    return gubg::file::exists(libsDir_) && gubg::file::exists(appsDir_);
}
bool GUBG::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
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
            compileSettings_.includePaths.insert(libsDir_);
        else if (bn == "da")
            compileSettings_.includePaths.insert(appsDir_);
        else
            return false;
    }

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
