//#define GUBG_LOG
#include "da/package/Local.hpp"
#include "gubg/file/Filesystem.hpp"
#include <sstream>
using namespace da::package;
using namespace gubg::file;

Local::Local(const File &dir):
    dir_(dir)
{
    File d(dir);
    if (d.popBasename(basename_))
        forest_.add(dir_, {"cpp", "hpp"});
}

std::string Local::name() const
{
    std::ostringstream oss;
    oss << "local(" << basename_ << ")";
    return oss.str();
}

bool Local::exists() const
{
    return gubg::file::exists(dir_);
}
bool Local::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
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
        if (bn == basename_)
            compileSettings_.includePaths.insert(root);
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
