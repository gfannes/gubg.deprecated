#include "da/package/DecodeIt.hpp"
#include "da/Arduino.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "DecodeIt"
#include "gubg/log/begin.hpp"
DecodeIt::DecodeIt(const File &base):
    base_(base),
    softDir_(base)
{
    softDir_ << "soft";

    File dci(softDir_);
    dci << "dci";
    forest_.add(dci, {"cpp", "hpp"});
}

bool DecodeIt::exists() const
{
    return gubg::file::exists(base_);
}

da::ReturnCode DecodeIt::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode, partial.name());

    File root;
    MSS_Q(forest_.resolve(resolvedHeader, root, partial, 1), UnknownHeader);
    L("Found this header in my forest");

    {
        string bn;
        MSS_Q(root.popBasename(bn), UnknownHeader);
        if (bn == "dci")
        {
            if (compileSettings_.targetPlatform == Any)
            {
                compileSettings_.targetPlatform = Host;
                linkSettings_.targetPlatform = Host;
            }
            compileSettings_.includePaths.insert(softDir_);
        }
        else
            MSS_QL(UnknownHeader);
    }

    auto sisterFile = resolvedHeader;
    sisterFile.setExtension("cpp");
    if (forest_.contains(sisterFile))
        sisterFiles.insert(sisterFile);

    MSS_END();
}
