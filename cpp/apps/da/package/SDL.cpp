#include "da/package/SDL.hpp"
#include "gubg/file/Filesystem.hpp"
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "SDL"
#include "gubg/log/begin.hpp"
SDL::SDL(const File &base):
    base_(base),
    libDir_(base)
{
    libDir_ << "stage/lib";
}

bool SDL::exists() const
{
    return true;
}

da::ReturnCode SDL::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode);

    File p = partial;
    string root;
    MSS_Q(p.popRoot(root), UnknownHeader);
    MSS_Q(root == "SDL", UnknownHeader);

    linkSettings_.libraries.insert("SDL");
    MSS_QL(RecognisedHeader);

    MSS_END();
}
