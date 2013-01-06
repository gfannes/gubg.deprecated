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

    File da(appsDir_);
    da << "da";
    forest_.add(da, {"cpp", "hpp"});
}

bool GUBG::exists() const
{
    return gubg::file::exists(libsDir_) && gubg::file::exists(appsDir_);
}
bool GUBG::resolveHeader(File &resolvedHeader, File &includePath, SourceFiles &sisterFiles, const File &partial) const
{
    File root;
    if (ReturnCode::OK != resolve(resolvedHeader, root, partial, 1))
        return false;

    {
        std::string bn;
        if (!root.popBasename(bn))
            return false;
        if (bn == "gubg")
            includePath = libsDir_;
        else if (bn == "da")
            includePath = appsDir_;
        else
            return false;
    }

    {
        auto sisterFile = resolvedHeader;
        sisterFile.setExtension("cpp");
        if (forest_.contains(sisterFile))
            sisterFiles.insert(sisterFile);
    }
    return true;
}
