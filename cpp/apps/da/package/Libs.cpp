#include "da/package/Libs.hpp"
#include "gubg/file/Filesystem.hpp"
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
