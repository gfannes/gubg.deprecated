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

da::ReturnCode Local::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode);

    File root;
    MSS_Q(forest_.resolve(resolvedHeader, root, partial, 1), UnknownHeader);

    {
        std::string bn;
        MSS_Q(root.popBasename(bn), UnknownHeader);
        MSS_Q(bn == basename_, UnknownHeader);
        compileSettings_.includePaths.insert(root);
    }

    {
        auto sisterFile = resolvedHeader;
        sisterFile.setExtension("cpp");
        if (forest_.contains(sisterFile))
            sisterFiles.insert(sisterFile);
    }

    MSS_END();
}
