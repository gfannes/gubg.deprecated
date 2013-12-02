#include "da/package/PkgConfig.hpp"
#include "gubg/string_algo.hpp"
#include <stdio.h>
using namespace da;
using namespace gubg::string_algo;
using namespace std;

#define GUBG_MODULE "PkgConfig"
#include "gubg/log/begin.hpp"
ReturnCode da::package::insertIncludePathsForPackage(IncludePaths &ips, const std::string &pkg)
{
    MSS_BEGIN(ReturnCode);

    FILE *fpipe;
    ostringstream command;
    command << "pkg-config --cflags " << pkg;

    MSS(fpipe = (FILE*)popen(command.str().c_str(), "r"));

    ostringstream oss;
    char line[256];
    while (fgets( line, sizeof line, fpipe))
        oss << line;
    pclose(fpipe);

    auto parts = split<vector>(oss.str(), " ");
    for (auto part: parts)
    {
        if (part.substr(0, 2) == "-I")
            ips.insert(part.substr(2));
    }

    MSS_END();
}
ReturnCode da::package::insertLibrariesForPackage(Libraries &libraries, const std::string &pkg)
{
    MSS_BEGIN(ReturnCode);

    FILE *fpipe;
    ostringstream command;
    command << "pkg-config --libs " << pkg;

    MSS(fpipe = (FILE*)popen(command.str().c_str(), "r"));

    ostringstream oss;
    char line[256];
    while (fgets( line, sizeof line, fpipe))
        oss << line;
    pclose(fpipe);

    auto parts = split<vector>(oss.str(), " ");
    for (auto part: parts)
    {
        if (part.substr(0, 2) == "-l")
            libraries.insert(part.substr(2));
    }

    MSS_END();
}
#include "gubg/log/end.hpp"
