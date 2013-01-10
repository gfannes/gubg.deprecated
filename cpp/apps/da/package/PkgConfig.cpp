#include "da/package/PkgConfig.hpp"
#include "gubg/string_algo.hpp"
#include <stdio.h>
using namespace da;
using namespace gubg::string_algo;
using namespace std;

ReturnCode da::package::getIncludePathsForPackage(IncludePaths &ips, const std::string &pkg)
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
        MSS(part.substr(0, 2) == "-I");
        ips.insert(part.substr(2));
    }

    MSS_END();
}
