#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/parse/cpp/Parser.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace gubg::parse::cpp;
using namespace std;

ReturnCode Parser::parse(const gubg::file::File &file)
{
    MSS_BEGIN(ReturnCode, parse);

    //Read file into range
    Range range;
    MSS(gubg::file::read(range, file));
    LOG_M("I read " << range.size() << " bytes");

    MSS_END();
}
