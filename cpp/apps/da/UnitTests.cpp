#include "da/UnitTests.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace da;

#define GUBG_MODULE_ "UnitTests"
#include "gubg/log/begin.hpp"
ReturnCode UnitTests::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode);
    L(gubg::file::getcwd());
    MSS_END();
}
#include "gubg/log/end.hpp"
