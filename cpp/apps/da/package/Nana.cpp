#include "da/package/Nana.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "Nana"
#include "gubg/log/begin.hpp"
Nana::Nana(const File &base):
    base_(base),
    libDir_(base),
    incDir_(base)
{
    libDir_ << "Nana.Cpp11/build/bin";
    incDir_ << "Nana.Cpp11/include";
}

bool Nana::exists() const
{
    SS();LL(base_.name());
    return gubg::file::exists(base_) && gubg::file::exists(libDir_) && gubg::file::exists(incDir_);
}

da::ReturnCode Nana::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode);

    File p = partial;
    string root;
    MSS_Q(p.popRoot(root), UnknownHeader);
    MSS_Q(root == "nana", UnknownHeader);

    L(partial.name());
    compileSettings_.includePaths.insert(incDir_);
    linkSettings_.libraries.insert("nana");
#ifdef GUBG_LINUX
    linkSettings_.libraries.insert("X11");
    linkSettings_.libraries.insert("rt");
    linkSettings_.libraries.insert("Xft");
    linkSettings_.libraries.insert("png");
    linkSettings_.libraries.insert("asound");
#endif
#ifdef GUBG_MINGW
    linkSettings_.libraries.insert("Gdi32");
#endif
    linkSettings_.libraryPaths.insert(libDir_);
    MSS_QL(RecognisedHeader);

    MSS_END();
}
