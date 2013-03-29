#include "da/package/Boost.hpp"
#include "gubg/file/Filesystem.hpp"
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "Local"
#include "gubg/log/begin.hpp"
Boost::Boost(const File &base):
    base_(base),
    libDir_(base)
{
    libDir_ << "stage/lib";
}

bool Boost::exists() const
{
    SS(base_.name());
    return gubg::file::exists(base_);
}

da::ReturnCode Boost::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode);

    File p = partial;
    string root;
    MSS_Q(p.popRoot(root), UnknownHeader);
    MSS_Q(root == "boost", UnknownHeader);

    L(partial.name());
    compileSettings_.includePaths.insert(base_);
    linkSettings_.libraries.insert("boost_thread");
    linkSettings_.libraries.insert("boost_system");
    linkSettings_.libraries.insert("boost_filesystem");
    linkSettings_.libraries.insert("boost_regex");
    linkSettings_.libraries.insert("boost_signals");
    linkSettings_.libraryPaths.insert(libDir_);
    MSS_QL(RecognisedHeader);

    MSS_END();
}
