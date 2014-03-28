#include "da/package/Libs.hpp"
#include "da/package/PkgConfig.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "Libs"
#include "gubg/log/begin.hpp"
Libs::Libs(const File &base):
    base_(base)
{
}

bool Libs::exists() const
{
    return true;
}

da::ReturnCode Libs::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode, partial.name());

    if (partial.name() == "ev++.h")
    {
        linkSettings_.libraries.insert("ev");
        MSS_QL(RecognisedHeader);
    }
    if (partial.name() == "gtkmm.h")
    {
        const auto pkg = "gtkmm-3.0";
        MSS(insertIncludePathsForPackage(compileSettings_.includePaths, pkg));
        MSS(insertLibrariesForPackage(linkSettings_.libraries, pkg));
        MSS_QL(RecognisedHeader);
    }
    if (partial.name() == "GL/gl.h")
    {
        linkSettings_.libraries.insert("GL");
        MSS_QL(RecognisedHeader);
    }
    if (partial.name() == "GL/glut.h")
    {
        linkSettings_.libraries.insert("glut");
        MSS_QL(RecognisedHeader);
    }
#ifdef GUBG_MINGW
    if (partial.name() == "Winsock2.h")
    {
        linkSettings_.libraries.insert("Ws2_32");
        MSS_QL(RecognisedHeader);
    }
#endif
    MSS_QL(UnknownHeader);
    /*
    File p = partial;
    string root;
    MSS_Q(p.popRoot(root), UnknownHeader);
    MSS_Q(root == "Libs", UnknownHeader);

    linkSettings_.libraries.insert("Libs");
    */

    MSS_END();
}
